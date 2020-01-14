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
	startSurveillance();
}

KinovaArm::KinovaArm(Coordinates homePosition, Logger logger)
    : homePosition{homePosition}
    , logger{logger} {
	startSurveillance();
}

KinovaArm::~KinovaArm() {
	stopSurveillance();
	stopUpdateLoop();
	if (hasControl) {
		releaseControl();
	}
	disconnect();
}

auto KinovaArm::connect() -> bool {
	try {
		arm.emplace();
		logInfo("connect", "successfully connected to arm.");
		isInFailState = false;
		startUpdateLoop();
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
		targetPosition = position;
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

	if ((currentSteeringMode != mode || mode == SteeringMode::NoMode)) {
		if (!canChangeMode()) {
			return false;
		}

		try {
			if ((currentSteeringMode != SteeringMode::Axis1 && currentSteeringMode != SteeringMode::Axis2) &&
			    (mode == SteeringMode::Axis1 || mode == SteeringMode::Axis2)) {
				arm->set_control_ang();
				lastSteeringModeChange = std::chrono::steady_clock::now();
			}
			if ((currentSteeringMode != SteeringMode::Translation && currentSteeringMode != SteeringMode::Rotation) &&
			    (mode == SteeringMode::Translation || mode == SteeringMode::Rotation) && canChangeMode()) {
				arm->set_control_cart();
				lastSteeringModeChange = std::chrono::steady_clock::now();
			}
		} catch (std::exception const &e) {
			logError("setSteeringMode", "failed to set steering mode. reason: {0}", e.what());
			return false;
		}

		currentSteeringMode = mode;
	} else if (mode == SteeringMode::NoMode) {
		currentSteeringMode = SteeringMode::Translation;
	}

	return true;
} // namespace KinovaZED::Hw

auto KinovaArm::hasFailed() const -> bool {
	return isInFailState;
}

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
				updateSteeringMode();
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
		try {
			auto rawPosition = arm->get_cart_pos();
			for (auto coordinateIndex{0}; coordinateIndex < 3; ++coordinateIndex) {
				currentPosition[coordinateIndex] = rawPosition.s.position[coordinateIndex];
				currentPosition[coordinateIndex + 3] = rawPosition.s.rotation[coordinateIndex];
			}
			if (currentPosition == targetPosition) {
				targetPosition.reset();
				if (isHoming) {
					isHoming = false;
					wasHomed = true;
				}
				// fire position reached
			}
		} catch (std::exception const &e) {
			logError("<updatePosition>", "failed to read position from arm. reason: {0}", e.what());
		}
	}
}

auto KinovaArm::updateRetractionMode() -> void {
	if (arm) {
		try {
			currentRetractionMode = static_cast<RetractionMode>(arm->get_status());
			if (currentRetractionMode == targetRetractionMode) {
				if (isHoming && currentRetractionMode == RetractionMode::ReadyToStandby) {
					targetRetractionMode.reset();
					arm->release_joystick();
					moveToSoftwareHome();
				}
			}
		} catch (std::exception const &e) {
			logError("<updateRetractionMode>", "failed to read retractionMode. reason: {0}", e.what());
		}
	}
}

auto KinovaArm::updateSteeringMode() -> void {
	using namespace std::chrono_literals;
	if (lastSteeringModeChange != std::chrono::steady_clock::time_point{}) {
		if ((std::chrono::steady_clock::now() - lastSteeringModeChange) >= 500ms) {
			logInfo("<updateSteeringMode>", "desired stearing mode was reached.");
			lastSteeringModeChange = std::chrono::steady_clock::time_point{};
			// fireModeChanged();
		}
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
	}
}

auto KinovaArm::moveToHardwareHome() -> void {
	switch (currentRetractionMode) {
	case RetractionMode::RetractToReady:
		targetRetractionMode = RetractionMode::ReadyToStandby;
		arm->push_joystick_button(2);
		arm->release_joystick();
		arm->push_joystick_button(2);
		break;
	case RetractionMode::NormalToReady:
	case RetractionMode::ReadyToRetract:
	case RetractionMode::RetractToStandby:
	case RetractionMode::Normal:
	case RetractionMode::NoInitToReady:
		targetRetractionMode = RetractionMode::ReadyToStandby;
		arm->push_joystick_button(2);
		break;
	case RetractionMode::Error:
		logError("moveToHardwareHome", "arm is in a failure state.");
		break;
	default:
		logDebug("moveToHardwareHome", "the arm is already in the hardware home position.");
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
	switch (currentRetractionMode) {
	case RetractionMode::ReadyToRetract:
		targetRetractionMode = RetractionMode::RetractToStandby;
		arm->push_joystick_button(2);
		arm->release_joystick();
		arm->push_joystick_button(2);
		break;
	case RetractionMode::ReadyToStandby:
	case RetractionMode::RetractToReady:
		targetRetractionMode = RetractionMode::RetractToStandby;
		arm->push_joystick_button(2);
		break;
	case RetractionMode::NormalToReady:
	case RetractionMode::Normal:
	case RetractionMode::NoInitToReady:
		logError("moveToRetractionPoint",
		         "cannot retract from current mode. mode: {0}",
		         static_cast<int>(currentRetractionMode));
		break;
	case RetractionMode::Error:
		logError("moveToRetractionPoint", "arm is in a failure state.");
		break;
	default:
		logDebug("moveToRetractionPoint", "the arm is already retracted.");
		break;
	}
}

auto KinovaArm::canChangeMode() -> bool {
	using namespace std::chrono_literals;
	return (std::chrono::steady_clock::now() - lastSteeringModeChange) >= 500ms;
}

} // namespace KinovaZED::Hw