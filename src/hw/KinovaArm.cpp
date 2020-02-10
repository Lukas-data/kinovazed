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

using namespace std::chrono_literals;

namespace KinovaZED::Hw {

constexpr auto positionRange = 0.05;
constexpr auto rotationRange = 0.25;
constexpr auto velocityRange = 0.000002;
constexpr auto joystickCalcFactor = 0.0025f;


KinovaArm::KinovaArm(Logger logger)
    : LoggingMixin{logger, "KinovaArm"}
    , homePosition{} {
}

KinovaArm::KinovaArm(Coordinates homePosition, Logger logger)
    : LoggingMixin{logger, "KinovaArm"}
    , homePosition{homePosition} {
}

KinovaArm::~KinovaArm() {
	stopUpdateLoop();
	if (hasControl) {
		releaseControl();
	}
	disconnect();
}

auto KinovaArm::connect() -> bool {
	auto guard = std::lock_guard{accessLock};

	try {
		arm.emplace();
		logInfo("connect", "successfully connected to arm.");
		isInFailState = false;
		currentPosition = readPosition();
		retractionMode = readRetractionMode();
		steeringMode.reset();
		if (!runUpdateLoop) {
			startUpdateLoop();
		}
		return true;
	} catch (std::exception const &e) {
		logError("connect", "failed to connect to the arm. reason: {0}", e.what());
	}

	isInFailState = true;
	return false;
}

auto KinovaArm::disconnect() -> void {
	auto guard = std::lock_guard{accessLock};

	try {
		if (hasControl) {
			releaseControl();
		}
		if (arm) {
			arm.reset();
			logInfo("disconnect", "closed connection to arm.");
		}
	} catch (std::exception const &e) {
		logError("disconnect", "failed to close the connection to the arm. reason: {0}", e.what());
	}
}

auto KinovaArm::takeControl() -> bool {
	assert(arm);

	auto guard = std::lock_guard{accessLock};

	try {
		arm->start_api_ctrl();
		hasControl = true;

		if (!hasFailed()) {
			logInfo("takeControl", "gained control over the arm.");
		}
		return !hasFailed();
	} catch (std::exception const &e) {
		logError("takeControl", "failed to take API control over the arm. reason: {0}", e.what());
	}
	return false;
}

auto KinovaArm::releaseControl() -> bool {
	assert(arm);

	auto guard = std::lock_guard{accessLock};

	try {
		arm->stop_api_ctrl();
		logInfo("releaseControl", "released API control over the arm.");
		hasControl = false;
		return true;
	} catch (std::exception const &e) {
		logError("releaseControl", "failed to release API control over the arm. reason: {0}", e.what());
	}
	return false;
}

auto KinovaArm::initialize() -> void {
	auto guard = std::lock_guard{accessLock};

	try {
		auto retractionMode = readRetractionMode();
		movementStatus = MovementStatus::Initializing;

		if (retractionMode == RetractionMode::NoInitToReady) {
			logInfo("initialize", "initializing the arm, this might take some time.");
			pushButton(2);
		} else {
			logInfo("initialize", "the arm seems to be initialized already. going home instead");
			moveToHardwareHome();
		}

	} catch (std::exception const &e) {
		logError("initialize", "failed to initialize the arm. reason: {0}", e.what());
	}
}

auto KinovaArm::stopMoving() -> bool {
	assert(arm);
	assert(hasControl);

	auto guard = std::lock_guard{accessLock};

	try {
		arm->erase_trajectories();
		std::this_thread::sleep_for(10ms);
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
		std::this_thread::sleep_for(10ms);
		releaseJoystick();
		logInfo("stopMoving", "successfully released the joystick.");
		movementStatus.reset();
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

	if (homePosition) {
		logInfo("home", "moving towards software home.");
		movementStatus = MovementStatus::HomingToSoftwareHome;
		moveToSoftwareHome();
	} else {
		logInfo("home", "moving towards hardware home.");
		movementStatus = MovementStatus::HomingToHardwareHome;
		moveToHardwareHome();
	}
}

auto KinovaArm::retract() -> void {
	assert(arm);
	assert(hasControl);

	auto guard = std::lock_guard{accessLock};

	moveToRetractionPoint();
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
		movementStatus = MovementStatus::MovingToPosition;
		targetPosition = position;
	} catch (std::exception const &e) {
		logError("moveTo", "failed to move to desired position. reason: {0}", e.what());
	}
}

auto KinovaArm::setJoystick(int x, int y, int z) -> void {
	assert(arm);

	auto guard = std::lock_guard{accessLock};

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

	if (steeringMode == SteeringMode::Translation || steeringMode == SteeringMode::Axis1) {
		rawPosition.s.trans_lr = speedX;
		rawPosition.s.trans_fb = speedY;
		rawPosition.s.trans_rot = speedZ;
	} else if (steeringMode == SteeringMode::Rotation || steeringMode == SteeringMode::Axis2) {
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

	auto guard = std::lock_guard{accessLock};

	if (mode != SteeringMode::NoMode && !canChangeMode()) {
		logWarning("setSteeringMode",
		           "rejected steering mode change. reason: not enough time elapsed since last change");
		return false;
	}

	try {
		if (mode == SteeringMode::Axis1 || mode == SteeringMode::Axis2) {
			arm->set_control_ang();
		} else {
			arm->set_control_cart();
		}
	} catch (std::exception const &e) {
		logError("setSteeringMode", "failed to set steering mode. reason: {0}", e.what());
		return false;
	}

	releaseJoystick();
	steeringMode = mode;

	if (mode != SteeringMode::NoMode) {
		lastSteeringModeChange = std::chrono::steady_clock::now();
	} else {
		fireSteeringModeChanged(mode);
	}

	return true;
}

auto KinovaArm::hasFailed() const -> bool {
	return isInFailState;
}

} // namespace KinovaZED::Hw