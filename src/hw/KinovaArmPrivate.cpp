#include "hw/KinovaArm.h"

#include <asio/dispatch.hpp>
#include <asio/post.hpp>

#include <algorithm>
#include <cassert>
#include <future>
#include <iterator>
#include <thread>

using namespace std::chrono_literals;

namespace KinovaZED::Hw {

KinovaArm::KinovaArm(std::optional<Coordinates> origin, Logger logger)
    : LoggingMixin{logger, "KinovaArm"}
    , homePosition{origin}
    , ioWorkGuard{asio::make_work_guard(ioContext)} {
	ioRunner = std::async(std::launch::async, [this] { ioContext.run(); });
}

auto KinovaArm::scheduleStateUpdate() -> void {
	stateUpdateTimer.expires_from_now(10ms);
	stateUpdateTimer.async_wait([this](auto error) {
		if (!error) {
			scheduleStateUpdate();
			performStateUpdate();
		}
	});
}

auto KinovaArm::performStateUpdate() -> void {
	updatePosition();
	updateRetractionMode();
	if (state->lastSteeringModeChange) {
		updateSteeringMode();
	}
	checkCurrents();
	checkMovement();
	reconnectOnError();
}

auto KinovaArm::stopUpdateLoop() -> void {
	stateUpdateTimer.cancel();
}

auto KinovaArm::checkCurrents() -> void {
	try {
		auto rawCurrents = arm->get_ang_current();
		state->fullCurrent = std::accumulate(std::cbegin(rawCurrents.joints), std::cend(rawCurrents.joints), .0f);

		if (state->fullCurrent > 2) {
			logDebug("checkCurrents",
			         "({0}, {1}, {2}, {3}, {4}, {5})",
			         rawCurrents.joints[0],
			         rawCurrents.joints[1],
			         rawCurrents.joints[2],
			         rawCurrents.joints[3],
			         rawCurrents.joints[4],
			         rawCurrents.joints[5]);
		}

		if (state->fullCurrent > 4.5f) {
			logError("checkCurrents", "overcurrent detected. Shutting down the arm. current: {0}", state->fullCurrent);
			disconnect();
		}
	} catch (std::exception const &e) {
		logError("updateCurrents", "failed to read currents from arm. reason: {0}", e.what());
	}
}

auto KinovaArm::checkMovement() -> void {
	auto velocities = arm->get_ang_vel();
	auto isStopped = std::all_of(
	    std::cbegin(velocities.joints), std::cend(velocities.joints), [](auto velocity) { return velocity == 0.0; });

	if (!isStopped) {
		return;
	}

	if (state->movementStatus == MovementStatus::MovingToPosition && state->targetPosition) {
		logWarning("checkMovement", "the arm stopped moving before reaching its target point!");
	}
}

auto KinovaArm::updatePosition() -> void {
	try {
		auto position = readPosition();
		if (position == state->currentPosition) {
			return;
		}

		handlePosition(position);
		state->currentPosition = position;
	} catch (std::exception const &e) {
		logError("<updatePosition>", "failed to read position from arm. reason: {0}", e.what());
	}
}

auto KinovaArm::readPosition() -> Coordinates {
	auto rawPosition = arm->get_cart_pos().s;

	auto [x, y, z] = rawPosition.position;
	auto [pitch, yaw, roll] = rawPosition.rotation;

	return {x, y, z, pitch, yaw, roll};
}

auto KinovaArm::handlePosition(Coordinates newPosition) -> void {
	if (state->movementStatus == MovementStatus::HomingToSoftwareHome && newPosition == homePosition) {
		state->retractionStatus = RetractionStatus::Homed;
		state->movementStatus.reset();
		std::this_thread::sleep_for(10ms);
		asio::post(actionStrand, [this] { fireHomeReached(); });
		return;
	}

	if (state->movementStatus == MovementStatus::MovingToPosition && isInRange(newPosition, *state->targetPosition)) {
		state->targetPosition.reset();
		state->movementStatus.reset();
		std::this_thread::sleep_for(10ms);
		asio::post(actionStrand, [this, newPosition] { firePositionReached(newPosition); });
		return;
	}
}

auto KinovaArm::updateRetractionMode() -> void {
	try {
		auto mode = readRetractionMode();

		if (mode == state->retractionMode) {
			return;
		}
		handleRetractionMode(mode);
		state->retractionMode = mode;
	} catch (std::exception const &e) {
		logError("<updateRetractionMode>", "failed to read retractionMode. reason: {0}", e.what());
	}
}

auto KinovaArm::readRetractionMode() -> RetractionMode {
	auto status = arm->get_status();
	return static_cast<RetractionMode>(status);
}

auto KinovaArm::handleRetractionMode(RetractionMode newMode) -> void {
	logDebug("<updateRetractionMode>", "reached retraction mode {0};", static_cast<int>(newMode));
	if (state->movementStatus) {
		logDebug("<updateRetractionMode>", "movement status is {0};", static_cast<int>(*state->movementStatus));
	}

	if (newMode == RetractionMode::ReadyToStandby) {
		logDebug("<updateRetractionMode>", "ready to standby");
		state->wasHomed = true;
		if (state->movementStatus == MovementStatus::HomingToHardwareHome) {
			state->movementStatus.reset();
			releaseJoystick();
			asio::post(actionStrand, [this] { fireHomeReached(); });
			return;
		} else if (state->movementStatus == MovementStatus::HomingToSoftwareHome) {
			state->movementStatus.reset();
			releaseJoystick();
			moveToSoftwareHome();
			return;
		} else if (state->movementStatus == MovementStatus::Initializing) {
			state->movementStatus.reset();
			releaseJoystick();
			asio::post(actionStrand, [this] { fireInitializationFinished(); });
			return;
		} else if (state->movementStatus == MovementStatus::HomeToRetract) {
			releaseJoystick();
			moveToRetractionPoint();
		}
	}

	if (newMode == RetractionMode::RetractToStandby) {
		state->wasHomed = false;
		if (state->movementStatus == MovementStatus::Retracting ||
		    state->movementStatus == MovementStatus::Initializing) {
			state->movementStatus.reset();
			releaseJoystick();
			asio::post(actionStrand, [this] { fireRetractionPointReached(); });
			return;
		}
	}
}

auto KinovaArm::updateSteeringMode() -> void {
	using namespace std::chrono_literals;

	if ((std::chrono::steady_clock::now() - *state->lastSteeringModeChange) >= 500ms) {
		logInfo("<updateSteeringMode>", "desired steering mode was reached.");
		state->lastSteeringModeChange.reset();
		asio::post(actionStrand, [this] { fireSteeringModeChanged(*state->steeringMode); });
	}
}

auto KinovaArm::reconnectOnError() -> void {
	if (!(hasFailed() && shouldReconnectOnError())) {
		return;
	}

	auto hadControl = state->hasControl;

	while (!connect()) {
		logWarning("reconnectOnError", "Trying to reconnect after a critical error");
		std::this_thread::sleep_for(50ms);
	}

	if (hadControl) {
		takeControl();
		stopMoving();
	}

	asio::post(actionStrand, [this] { fireReconnectedDueToError(); });
}

auto KinovaArm::moveToHardwareHome() -> void {
	logDebug("moveToHardwareHome",
	         "trying to move to hardware home. retraction mode: {0}",
	         static_cast<int>(*state->retractionMode));

	try {
		switch (*state->retractionMode) {
		case RetractionMode::RetractToReady:
			pushButton(2);
			releaseJoystick();
			pushButton(2);
			break;
		case RetractionMode::NormalToReady:
		case RetractionMode::ReadyToRetract:
		case RetractionMode::RetractToStandby:
		case RetractionMode::Normal:
		case RetractionMode::NoInitToReady:
			pushButton(2);
			break;
		case RetractionMode::Error:
			logError("moveToHardwareHome", "arm is in a failure state.");
			break;
		default:
			if (state->movementStatus == MovementStatus::HomingToHardwareHome) {
				state->movementStatus.reset();
				asio::post(actionStrand, [this] { fireHomeReached(); });
			} else if (state->movementStatus == MovementStatus::Initializing) {
				state->movementStatus.reset();
				asio::post(actionStrand, [this] { fireInitializationFinished(); });
			}
			break;
		}
	} catch (std::exception const &e) {
		logError("moveToHardwareHome", "failed to move to hardware home. reason: {0}", e.what());
	}
} // namespace KinovaZED::Hw

auto KinovaArm::moveToSoftwareHome() -> void {
	if (state->currentPosition == homePosition) {
		state->movementStatus.reset();
		asio::post(actionStrand, [this] { fireHomeReached(); });
		return;
	} else if (state->wasHomed) {
		moveTo(*homePosition);
		return;
	}

	moveToHardwareHome();
}

auto KinovaArm::moveToRetractionPoint() -> void {
	state->movementStatus = MovementStatus::Retracting;
	state->retractionMode = readRetractionMode();

	try {
		switch (*state->retractionMode) {
		case RetractionMode::ReadyToRetract:
			pushButton(2);
			// HACK: The arm does not realize we pressed the button if we don't wait longer than usual
			std::this_thread::sleep_for(200ms);
			releaseJoystick();
			pushButton(2);
			break;
		case RetractionMode::ReadyToStandby:
		case RetractionMode::RetractToReady:
			pushButton(2);
			break;
		case RetractionMode::NormalToReady:
		case RetractionMode::Normal:
		case RetractionMode::NoInitToReady:
			logWarning("moveToRetractionPoint",
			           "cannot retract from current mode. mode: {0}",
			           static_cast<int>(*state->retractionMode));
			state->movementStatus = MovementStatus::HomeToRetract;
			pushButton(2);
			break;
		case RetractionMode::Error:
			logError("moveToRetractionPoint", "arm is in a failure state.");
			break;
		default:
			if (state->movementStatus == MovementStatus::Retracting) {
				state->movementStatus.reset();
				asio::post(actionStrand, [this] { fireRetractionPointReached(); });
			}
			break;
		}
	} catch (std::exception const &e) {
		logError("moveToRetractionPoint", "failed to retract the arm. reason: {0}", e.what());
	}
}

auto KinovaArm::canChangeMode() -> bool {
	using namespace std::chrono_literals;
	return !state->lastSteeringModeChange ||
	    (std::chrono::steady_clock::now() - *state->lastSteeringModeChange) >= 500ms;
}

auto KinovaArm::pushButton(int index) -> void {
	logDebug("pushButton", "pushing button {0}", index);
	arm->push_joystick_button(index);
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
}

auto KinovaArm::releaseJoystick() -> void {
	logDebug("releaseJoystick", "releasing joystick");
	arm->release_joystick();
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
}

auto KinovaArm::moveJoystick(KinDrv::jaco_joystick_axis_t position) -> void {
	logDebug("moveJoystick", "moving joystick");
	arm->move_joystick_axis(position);
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
}

auto KinovaArm::eraseTrajectories() -> void {
	logDebug("eraseTrajectories", "erasing all trajectories");
	arm->erase_trajectories();
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
}

} // namespace KinovaZED::Hw