#include "hw/KinovaArm.h"

#include <algorithm>
#include <future>
#include <thread>

using namespace std::chrono_literals;

namespace KinovaZED::Hw {

auto KinovaArm::startUpdateLoop() -> void {
	runUpdateLoop = true;
	updateLoopHandle = std::async(std::launch::async, [this] {
		while (runUpdateLoop) {
			{
				auto guard = std::lock_guard{accessLock};
				if (arm) {
					updatePosition();
					updateRetractionMode();
					if (state->lastSteeringModeChange) {
						updateSteeringMode();
					}
					checkCurrents();
				}
			}
			reconnectOnError();
			std::this_thread::sleep_for(10ms);
		}
	});
}

auto KinovaArm::stopUpdateLoop() -> void {
	if (runUpdateLoop) {
		runUpdateLoop = false;
		updateLoopHandle.get();
	}
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

auto KinovaArm::updatePosition() -> void {
	try {
		auto position = readPosition();

		if (position == state->currentPosition) {
			return;
		}

		state->currentPosition = position;

		if (state->movementStatus == MovementStatus::HomingToSoftwareHome && state->currentPosition == homePosition) {
			state->retractionStatus = RetractionStatus::Homed;
			state->movementStatus.reset();
			std::this_thread::sleep_for(10ms);
			fireHomeReached();
			return;
		}

		if (state->movementStatus == MovementStatus::MovingToPosition &&
		    isInRange(*state->currentPosition, *state->targetPosition)) {
			state->targetPosition.reset();
			state->movementStatus.reset();
			std::this_thread::sleep_for(10ms);
			firePositionReached(*state->currentPosition);
			return;
		}
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

auto KinovaArm::updateRetractionMode() -> void {
	try {
		auto mode = readRetractionMode();

		if (mode == state->retractionMode) {
			return;
		}

		state->retractionMode = mode;
		logDebug("<updateRetractionMode>", "reached retraction mode {0};", static_cast<int>(*state->retractionMode));
		if (state->movementStatus) {
			logDebug("<updateRetractionMode>", "movement status is {0};", static_cast<int>(*state->movementStatus));
		}

		if (state->retractionMode == RetractionMode::ReadyToStandby) {
			logDebug("<updateRetractionMode>", "ready to standby");
			state->wasHomed = true;
			if (state->movementStatus == MovementStatus::HomingToHardwareHome) {
				state->movementStatus.reset();
				releaseJoystick();
				fireHomeReached();
				return;
			} else if (state->movementStatus == MovementStatus::HomingToSoftwareHome) {
				state->movementStatus.reset();
				releaseJoystick();
				moveToSoftwareHome();
				return;
			} else if (state->movementStatus == MovementStatus::Initializing) {
				state->movementStatus.reset();
				releaseJoystick();
				fireInitializationFinished();
				return;
			}
		}

		if (state->retractionMode == RetractionMode::RetractToStandby) {
			state->wasHomed = false;
			if (state->movementStatus == MovementStatus::Retracting ||
			    state->movementStatus == MovementStatus::Initializing) {
				state->movementStatus.reset();
				releaseJoystick();
				fireRetractionPointReached();
				return;
			}
		}
	} catch (std::exception const &e) {
		logError("<updateRetractionMode>", "failed to read retractionMode. reason: {0}", e.what());
	}
}

auto KinovaArm::readRetractionMode() -> RetractionMode {
	auto status = arm->get_status();
	return static_cast<RetractionMode>(status);
}

auto KinovaArm::updateSteeringMode() -> void {
	using namespace std::chrono_literals;

	if ((std::chrono::steady_clock::now() - *state->lastSteeringModeChange) >= 500ms) {
		logInfo("<updateSteeringMode>", "desired steering mode was reached.");
		state->lastSteeringModeChange.reset();
		fireSteeringModeChanged(*state->steeringMode);
	}
}

auto KinovaArm::reconnectOnError() -> void {
	{
		auto guard = std::lock_guard(accessLock);

		if (!(hasFailed() && shouldReconnectOnError())) {
			return;
		}
	}

	if (connect()) {
		if (state->hasControl) {
			takeControl();
			stopMoving();
		}

		auto guard = std::lock_guard(accessLock);
		fireReconnectedDueToError();
	}
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
				fireHomeReached();
			} else if (state->movementStatus == MovementStatus::Initializing) {
				state->movementStatus.reset();
				fireInitializationFinished();
			}
			break;
		}
	} catch (std::exception const &e) {
		logError("moveToHardwareHome", "failed to move to hardware home. reason: {0}", e.what());
	}
}

auto KinovaArm::moveToSoftwareHome() -> void {
	if (state->currentPosition == homePosition) {
		state->movementStatus.reset();
		fireHomeReached();
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
			logError("moveToRetractionPoint",
			         "cannot retract from current mode. mode: {0}",
			         static_cast<int>(*state->retractionMode));
			break;
		case RetractionMode::Error:
			logError("moveToRetractionPoint", "arm is in a failure state.");
			break;
		default:
			if (state->movementStatus == MovementStatus::Retracting) {
				state->movementStatus.reset();
				fireRetractionPointReached();
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
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
	arm->push_joystick_button(index);
}

auto KinovaArm::releaseJoystick() -> void {
	logDebug("releaseJoystick", "releasing joystick");
	std::this_thread::sleep_for(std::chrono::milliseconds{10});
	arm->release_joystick();
}


} // namespace KinovaZED::Hw