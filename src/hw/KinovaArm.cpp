#include "hw/KinovaArm.h"

#include "hw/Coordinates.h"
#include "support/Logging.h"

#include "libkindrv/kindrv.h"

#include <array>
#include <cassert>
#include <chrono>
#include <exception>
#include <future>
#include <istream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <optional>

namespace KinovaZED::Hw {

constexpr auto positionRange = 0.05;
constexpr auto rotationRange = 0.25;
constexpr auto velocityRange = 0.000002;
constexpr auto joystickCalcFactor = 0.0025f;


KinovaArm::KinovaArm(Logger logger)
    : homePosition{}
    , logger{logger} {
}

KinovaArm::KinovaArm(Coordinates homePosition, Logger logger)
    : homePosition{homePosition}
    , logger{logger} {
}

KinovaArm::~KinovaArm() {
	disconnect();
}

auto KinovaArm::connect() -> bool {
	try {
		arm.emplace();
		logInfo("connect", "successfully connected to arm.");
		isInFailState = false;
		return true;
	} catch (std::exception const &e) {
		logError("connect", "failed to connect to the arm. reason: {0}", e.what());
	}
	isInFailState = true;
	return false;
}

auto KinovaArm::disconnect() -> void {
	try {
		arm.reset();
		logInfo("disconnect", "closed connection to arm.");
	} catch (std::exception const &e) {
		logError("disconnect", "failed to close the connection to the arm. reason: {0}", e.what());
	}
}

auto KinovaArm::takeControl() -> bool {
	assert(arm);
	try {
		arm->start_api_ctrl();
		logInfo("takeControl", "gained API control over the arm.");
		hasControl = true;
		return true;
	} catch (std::exception const &e) {
		logError("takeControl", "failed to take API control over the arm. reason: {0}", e.what());
	}
	return false;
}

auto KinovaArm::releaseControl() -> bool {
	assert(arm);
	try {
		arm->stop_api_ctrl();
		logInfo("releaseControl", "released API control over the arm.");
		hasControl = false;
		return true;
	} catch (std::exception const &e) {
		logError("takeControl", "failed to release API control over the arm. reason: {0}", e.what());
	}
	return false;
}

auto KinovaArm::stopMoving() -> bool {
	assert(arm);
	assert(hasControl);

	try {
		arm->erase_trajectories();
		logInfo("stopMoving", "erased all stored trajectories.");
	} catch (std::exception const &e) {
		logError("stopMoving", "failed to erase stored trajectories. reason: {0}", e.what());
	}

	auto joystickPosition = KinDrv::jaco_joystick_axis_t{};
	joystickPosition.s.trans_lr = 0;
	joystickPosition.s.trans_fb = 0;
	joystickPosition.s.trans_rot = 0;
	joystickPosition.s.wrist_lr = 0;
	joystickPosition.s.wrist_fb = 0;
	joystickPosition.s.wrist_rot = 0;

	try {
		arm->move_joystick_axis(joystickPosition);
		logInfo("stopMoving", "successfully put the joystick into the neutral position.");
		arm->release_joystick();
		logInfo("stopMoving", "successfully release the joystick.");
		return true;
	} catch (std::exception const &e) {
		logError("stop moving", "failed to release the joystick in the neutral position. reason: {}", e.what());
	}
	return false;
}

auto KinovaArm::home() -> void {
	assert(arm);
	assert(hasControl);

	auto guard = std::lock_guard{accessLock};

	if (currentPosition == homePosition) {
		return;
	}

	if (!wasHomed) {
		isHoming = true;
		moveToHardwareHome();
	} else {
		moveToSoftwareHome();
	}
}

auto KinovaArm::retract() -> void {
	assert(arm);
	assert(hasControl);

	auto guard = std::lock_guard{accessLock};

	if (currentRetractionMode == RetractionMode::RetractToStandby) {
		return;
	} else {
		moveToRetractionPoint();
	}
}

auto KinovaArm::moveTo(Coordinates position) -> void {
	assert(arm);

	auto guard = std::lock_guard{accessLock};

	if (fullCurrent > 3.5) {
		logWarning("moveTo", "high motor current detected, skipping movement. current: {0}", fullCurrent);
	}

	try {
		float fingerPositions[3] = {.0f, .0f, .0f};
		auto armPosition = static_cast<std::array<float, 6>>(position);
		arm->set_target_cart(armPosition.data(), fingerPositions);
	} catch (std::exception const &e) {
		logError("moveTo", "failed to move to desired position. reason: {0}", e.what());
	}
}

auto KinovaArm::setJoystick(int x, int y, int z) -> void {
	assert(arm);

	auto speedX = -x * joystickCalcFactor;
	auto speedY = -y * joystickCalcFactor;
	auto speedZ = z * joystickCalcFactor;

	auto rawPosition = KinDrv::jaco_joystick_axis_t{};
	rawPosition.s.trans_lr = 0;
	rawPosition.s.trans_fb = 0;
	rawPosition.s.trans_rot = 0;
	rawPosition.s.wrist_lr = 0;
	rawPosition.s.wrist_fb = 0;
	rawPosition.s.wrist_rot = 0;

	if (currentSteeringMode == SteeringMode::Translation || currentSteeringMode == SteeringMode::Axis1) {
		rawPosition.s.trans_lr = speedX;
		rawPosition.s.trans_fb = speedY;
		rawPosition.s.trans_rot = speedZ;
	} else if (currentSteeringMode == SteeringMode::Rotation || currentSteeringMode == SteeringMode::Axis2) {
		rawPosition.s.wrist_lr = speedX;
		rawPosition.s.wrist_fb = speedY;
		rawPosition.s.wrist_rot = speedZ;
	}

	try {
		arm->move_joystick_axis(rawPosition);
		logDebug("setJoystick", "setting joystick input to (x = {0}, y = {1}, z = {2})", speedX, speedY, speedZ);
	} catch (std::exception const &e) {
		logError("setJoystick", "failed to set joystick speeds. reason: {0}", e.what());
	}
}

auto KinovaArm::setSteeringMode(SteeringMode mode) -> bool {
	assert(arm);
	assert(isKnownSteeringMode(static_cast<int>(mode)));

	if (currentSteeringMode != mode || mode == SteeringMode::NoMode) {
		try {
			if ((currentSteeringMode != SteeringMode::Axis1 && currentSteeringMode != SteeringMode::Axis2) &&
			    (mode == SteeringMode::Axis1 || mode == SteeringMode::Axis2)) {
				if (canChangeMode()) {
					arm->set_control_ang();
					lastSteeringModeChange = std::chrono::steady_clock::now();
				} else {
					return false;
				}
			}
			if ((currentSteeringMode != SteeringMode::Translation && currentSteeringMode != SteeringMode::Rotation) &&
			    (mode == SteeringMode::Translation || mode == SteeringMode::Rotation) && canChangeMode()) {
				if (canChangeMode()) {
					arm->set_control_cart();
					lastSteeringModeChange = std::chrono::steady_clock::now();
				} else {
					return false;
				}
			}
		} catch (std::exception const &e) {
			logError("setSteeringMode", "failed to set steering mode. reason: {0}", e.what());
			return false;
		}
	}
	switch (mode) {
	case SteeringMode::NoMode:
		currentSteeringMode = SteeringMode::Translation;
		break;
	case SteeringMode::Translation:
	case SteeringMode::Rotation:
	case SteeringMode::Axis1:
	case SteeringMode::Axis2:
		currentSteeringMode = mode;
		break;
	}
	return true;
}

auto KinovaArm::hasFailed() const -> bool {
	return isInFailState;
}

// /*Get-Functions*/
// auto KinovaArm::getError() -> bool {
// 	return Error;
// }
// auto KinovaArm::getInitialize() -> bool {
// 	return initialized;
// }
// auto KinovaArm::getActive() -> bool {
// 	return active;
// }
// auto KinovaArm::getMode() -> int {
// 	return static_cast<int>(mode);
// }
// auto KinovaArm::getCurrentPosition() -> int {
// 	return currentPosition;
// }
// auto KinovaArm::getCurrentPoint() -> int {
// 	return getCurrentPoint(teachTarget);
// }
// auto KinovaArm::getCurrentPoint(Control::ObjectiveId target) -> int {
// 	return PositionHandler.getSequence(target);
// }

// void KinovaArm::setActive() {
// 	active = true;
// }
// void KinovaArm::setInactive() {
// 	active = false;
// 	mode = SteeringMode::NoMode;
// }

// /*Returns true if arm isn't moving anymore. Starts after initial timer of 500ms to wait for arm to start
// moving.*/ auto KinovaArm::checkIfReached(float *currentCoordinates) -> bool { 	if (!moveTimerStart) {
// 		moveTimerStart = std::chrono::steady_clock::now();
// 	}

// 	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - *moveTimerStart) >
// 	    std::chrono::milliseconds{500}) {
// 		bool pointReached = true;
// 		logger->warn("KinovaArm::RangeCheck: following Axis do not pass:");
// 		for (int i = 0; i < 6; i++) {
// 			float dVel = fabs(currentCoordinates[i] - lastCoordinates[i]);
// 			if (dVel > velocityRange) {
// 				logger->debug("[i: {0} dVel: {1}", i, dVel);
// 				pointReached = false;
// 			}
// 			lastCoordinates[i] = currentCoordinates[i];
// 		}
// 		// Returns PointReached if PointReachedCount is higher than 3, to prevent errors.
// 		if (pointReached) {
// 			++pointReachedCount;
// 		} else {
// 			pointReachedCount = 0;
// 		}
// 		if (pointReachedCount > 3) {
// 			pointReachedCount = 0;
// #if __cplusplus >= 201703L
// 			moveTimerStart = std::nullopt;
// #else
// 			moveTimerStart = std::experimental::nullopt;
// #endif
// 			return pointReached;
// 		}
// 	}
// 	return false;
// }

/// @section Private Implementation

auto KinovaArm::startUpdateLoop() -> void {
	using namespace std::chrono_literals;

	runUpdateLoop = true;
	updateLoopHandle = std::async(std::launch::async, [this] {
		while (runUpdateLoop) {
			{
				auto guard = std::lock_guard{accessLock};
				checkCurrents();
				updatePosition();
				updateRetractionMode();
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
	if (arm) {
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
				logError("checkCurrents", "overcurrent detected. Shutting down the arm. current {0}", fullCurrent);
				arm.reset();
				stopUpdateLoop();
				isInFailState = true;
			}
		} catch (std::exception const &e) {
			logError("updateCurrents", "failed to read currents from arm. reason: {0}", e.what());
		}
	}
}

auto KinovaArm::updatePosition() -> void {
	if (arm) {
		auto rawPosition = arm->get_cart_pos();
		for (auto coordinateIndex{0}; coordinateIndex < 3; ++coordinateIndex) {
			currentPosition[coordinateIndex] = rawPosition.s.position[coordinateIndex];
			currentPosition[coordinateIndex + 3] = rawPosition.s.rotation[coordinateIndex];
		}
		// if (currentPosition == targetPosition)
	}
}

auto KinovaArm::updateRetractionMode() -> void {
	if (arm) {
		currentRetractionMode = static_cast<RetractionMode>(arm->get_status());
		// if (currentRetractionMode == targetRetractionMode)
	}
}

auto KinovaArm::updateSteeringMode() -> void {
	using namespace std::chrono_literals;
	if (lastSteeringModeChange != std::chrono::steady_clock::time_point{}) {
		if ((std::chrono::steady_clock::now() - lastSteeringModeChange) >= 500ms) {
			lastSteeringModeChange = std::chrono::steady_clock::time_point{};
			// fireModeChanged();
		}
	}
}

auto KinovaArm::moveToHardwareHome() -> void {
	switch (currentRetractionMode) {
	case RetractionMode::RetractToReady:
		arm->push_joystick_button(2);
		arm->release_joystick();
		arm->push_joystick_button(2);
		break;
	case RetractionMode::NormalToReady:
	case RetractionMode::ReadyToRetract:
	case RetractionMode::RetractToStandby:
	case RetractionMode::Normal:
	case RetractionMode::NoInitToReady:
		arm->push_joystick_button(2);
		break;
	case RetractionMode::Error:
		logError("moveToHarwareHome", "arm is in a failure state.");
		break;
	default:
		break;
	}
}

auto KinovaArm::moveToSoftwareHome() -> void {
	assert(wasHomed);

	if (!homePosition) {
		return;
	}

	moveTo(*homePosition);
}

auto KinovaArm::moveToRetractionPoint() -> void {
	// TODO(fmorgner): implement
}

auto KinovaArm::canChangeMode() -> bool {
	using namespace std::chrono_literals;
	return (std::chrono::steady_clock::now() - lastSteeringModeChange) >= 500ms;
}

} // namespace KinovaZED::Hw