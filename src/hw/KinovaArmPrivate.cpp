#include "hw/KinovaArm.h"

#include <algorithm>
#include <thread>
#include <future>

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
					if (lastSteeringModeChange) {
						updateSteeringMode();
					}
					checkCurrents();
				}
			}
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
		fullCurrent = std::accumulate(std::cbegin(rawCurrents.joints), std::cend(rawCurrents.joints), .0f);

		if (fullCurrent > 2) {
			logDebug("checkCurrents",
			         "({0}, {1}, {2}, {3}, {4}, {5})",
			         rawCurrents.joints[0],
			         rawCurrents.joints[1],
			         rawCurrents.joints[2],
			         rawCurrents.joints[3],
			         rawCurrents.joints[4],
			         rawCurrents.joints[5]);
		}

		if (fullCurrent > 4.5f) {
			logError("checkCurrents", "overcurrent detected. Shutting down the arm. current: {0}", fullCurrent);
			disconnect();
		}
	} catch (std::exception const &e) {
		logError("updateCurrents", "failed to read currents from arm. reason: {0}", e.what());
	}
}

auto KinovaArm::updatePosition() -> void {
	try {
		auto position = readPosition();

		if (position == currentPosition) {
			return;
		}

		currentPosition = position;

		if (movementStatus == MovementStatus::HomingToSoftwareHome && currentPosition == homePosition) {
			retractionStatus = RetractionStatus::Homed;
			movementStatus.reset();
            std::this_thread::sleep_for(10ms);
			fireHomeReached();
			return;
		}

		if (movementStatus == MovementStatus::MovingToPosition && currentPosition == targetPosition) {
			targetPosition.reset();
            std::this_thread::sleep_for(10ms);
			firePositionReached(*currentPosition);
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

	return {z, y, z, pitch, yaw, roll};
}

auto KinovaArm::updateRetractionMode() -> void {
	try {
		auto mode = readRetractionMode();

		if (mode == retractionMode) {
			return;
		}

		retractionMode = mode;
		logDebug("<updateRetractionMode>", "reached retraction mode {0};", static_cast<int>(*retractionMode));
		logDebug("<updateRetractionMode>", "movement status is {0};", static_cast<int>(*movementStatus));

		if (retractionMode == RetractionMode::ReadyToStandby) {
			logDebug("<updateRetractionMode>", "ready to standby");
			wasHomed = true;
			if (movementStatus == MovementStatus::HomingToHardwareHome) {
				releaseJoystick();
				fireHomeReached();
				return;
			} else if (movementStatus == MovementStatus::HomingToSoftwareHome) {
				releaseJoystick();
				moveToSoftwareHome();
				return;
			}
		}

		if (retractionMode == RetractionMode::RetractToStandby) {
			wasHomed = false;
			if (movementStatus == MovementStatus::Retracting) {
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

	if ((std::chrono::steady_clock::now() - *lastSteeringModeChange) >= 500ms) {
		logInfo("<updateSteeringMode>", "desired steering mode was reached.");
		lastSteeringModeChange.reset();
		fireSteeringModeChanged(*steeringMode);
	}
}

auto KinovaArm::startSurveillance() -> void {
	using namespace std::chrono_literals;

	runSurveillance = true;
	surveillanceHandle = std::async(std::launch::async, [this] {
		while (runSurveillance) {
			{
				auto guard = std::lock_guard{accessLock};
				reconnectOnError();
			}
			std::this_thread::sleep_for(10ms);
		}
	});
}

auto KinovaArm::stopSurveillance() -> void {
	if (runSurveillance) {
		runSurveillance = false;
		surveillanceHandle.get();
	}
}

auto KinovaArm::reconnectOnError() -> void {
	auto guard = std::lock_guard(accessLock);

	if (arm && hasFailed() && shouldReconnectOnError()) {
		logWarning("<reconnectOnError>", "reconnecting due to arm failure.");
		stopUpdateLoop();
		disconnect();
		connect();

		if (hasControl) {
			takeControl();
		}

		fireReconnectedDueToError();
	}
}

auto KinovaArm::moveToHardwareHome() -> void {
	logDebug("moveToHardwareHome",
	         "trying to move to hardware home. retraction mode: {0}",
	         static_cast<int>(*retractionMode));

	try {
		switch (*retractionMode) {
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
			if (movementStatus == MovementStatus::HomingToHardwareHome) {
				movementStatus.reset();
				fireHomeReached();
			}
			break;
		}
	} catch (std::exception const &e) {
		logError("moveToHardwareHome", "failed to move to hardware home. reason: {0}", e.what());
	}
}

auto KinovaArm::moveToSoftwareHome() -> void {
	if (currentPosition == homePosition) {
		movementStatus.reset();
		fireHomeReached();
		return;
	} else if (wasHomed) {
		moveTo(*homePosition);
		return;
	}

	moveToHardwareHome();
}

auto KinovaArm::moveToRetractionPoint() -> void {
	movementStatus = MovementStatus::Retracting;

	try {
		switch (*retractionMode) {
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
			         static_cast<int>(*retractionMode));
			break;
		case RetractionMode::Error:
			logError("moveToRetractionPoint", "arm is in a failure state.");
			break;
		default:
			if (movementStatus == MovementStatus::Retracting) {
				movementStatus.reset();
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
	return !lastSteeringModeChange || (std::chrono::steady_clock::now() - *lastSteeringModeChange) >= 500ms;
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