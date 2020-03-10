#include "hw/KinovaArm.h"

#include <asio/dispatch.hpp>

#include <algorithm>
#include <future>
#include <optional>

using namespace std::chrono_literals;

namespace KinovaZED::Hw {

constexpr auto joystickCalcFactor = 0.0025f;

auto KinovaArm::doConnect(std::promise<bool> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			arm.emplace();
			logInfo("connect", "successfully connected to arm.");
			state = VolatileState{};
			performStateUpdate();
			scheduleStateUpdate();
			token.set_value(true);
		} catch (std::exception const &e) {
			logError("connect", "failed to connect to the arm. reason: {0}", e.what());
			token.set_value(false);
		}
	});
}

auto KinovaArm::doDisconnect(std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			if (state->hasControl) {
				releaseControl();
			}
			arm.reset();
		} catch (std::exception const &e) {
			logError("disconnect", "failed to close the connection to the arm. reason: {0}", e.what());
		}
		token.set_value();
	});
}

auto KinovaArm::doTakeControl(std::promise<bool> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			arm->start_api_ctrl();
			state->hasControl = true;

			if (!hasFailed()) {
				logInfo("takeControl", "gained control over the arm.");
			}
		} catch (std::exception const &e) {
			logError("takeControl", "failed to take API control over the arm. reason: {0}", e.what());
		}
		token.set_value(!hasFailed());
	});
}

auto KinovaArm::doReleaseControl(std::promise<bool> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			arm->stop_api_ctrl();
			logInfo("releaseControl", "released API control over the arm.");
			state->hasControl = false;
			token.set_value(true);
		} catch (std::exception const &e) {
			logError("releaseControl", "failed to release API control over the arm. reason: {0}", e.what());
			token.set_value(false);
		}
	});
}

auto KinovaArm::doInitialize(std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			auto retractionMode = readRetractionMode();
			state->movementStatus = MovementStatus::Initializing;

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
		token.set_value();
	});
}

auto KinovaArm::doStopMoving(std::promise<bool> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		try {
			eraseTrajectories();
			logInfo("stopMoving", "erased all stored trajectories.");
		} catch (std::exception const &e) {
			logError("stopMoving", "failed to erase stored trajectories. reason: {0}", e.what());
		}

		auto joystickPosition = KinDrv::jaco_joystick_axis_t{{0, 0, 0, 0, 0, 0}};

		try {
			moveJoystick(joystickPosition);
			logInfo("stopMoving", "successfully put the joystick into the neutral position.");
			releaseJoystick();
			logInfo("stopMoving", "successfully released the joystick.");
			state->movementStatus.reset();
			token.set_value(true);
		} catch (std::exception const &e) {
			logError("stop moving", "failed to release the joystick in the neutral position. reason: {}", e.what());
			token.set_value(false);
		}
	});
}

auto KinovaArm::doHome(std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		if (homePosition) {
			logInfo("home", "moving towards software home.");
			state->movementStatus = MovementStatus::HomingToSoftwareHome;
			moveToSoftwareHome();
		} else {
			logInfo("home", "moving towards hardware home.");
			state->movementStatus = MovementStatus::HomingToHardwareHome;
			moveToHardwareHome();
		}
		token.set_value();
	});
}

auto KinovaArm::doRetract(std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		moveToRetractionPoint();
		token.set_value();
	});
}

auto KinovaArm::doMoveTo(Coordinates position, std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, position, token = std::move(token)]() mutable {
		if (state->fullCurrent > 3.5) {
			logWarning("moveTo", "high motor current detected, skipping movement. current: {0}", state->fullCurrent);
		}

		try {
			float fingerPositions[3] = {.0f, .0f, .0f};
			auto armPosition = static_cast<std::array<float, 6>>(position);
			arm->set_target_cart(armPosition.data(), fingerPositions);
			state->movementStatus = MovementStatus::MovingToPosition;
			state->targetPosition = position;
		} catch (std::exception const &e) {
			logError("moveTo", "failed to move to desired position. reason: {0}", e.what());
		}
		token.set_value();
	});
}

auto KinovaArm::doSetJoystick(int x, int y, int z, std::promise<void> token) -> void {
	asio::dispatch(actionStrand, [this, x, y, z, token = std::move(token)]() mutable {
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

		auto steeringMode = state->steeringMode;

		if (steeringMode == SteeringMode::XYZ || steeringMode == SteeringMode::Axis1to3) {
			rawPosition.s.trans_lr = speedX;
			rawPosition.s.trans_fb = speedY;
			rawPosition.s.trans_rot = speedZ;
		} else if (steeringMode == SteeringMode::Rotation || steeringMode == SteeringMode::Axis4to6) {
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

		token.set_value();
	});
}

auto KinovaArm::doSetSteeringMode(SteeringMode mode, std::promise<bool> token) -> void {
	asio::dispatch(actionStrand, [this, mode, token = std::move(token)]() mutable {
		if (mode != SteeringMode::NoMode && mode != SteeringMode::Freeze && !canChangeMode()) {
			logWarning("setSteeringMode",
			           "rejected steering mode change. reason: not enough time elapsed since last change");
			return false;
		}

		try {
			if (mode == SteeringMode::Axis1to3 || mode == SteeringMode::Axis4to6) {
				arm->set_control_ang();
			} else {
				arm->set_control_cart();
			}
		} catch (std::exception const &e) {
			logError("setSteeringMode", "failed to set steering mode. reason: {0}", e.what());
			return false;
		}

		releaseJoystick();
		state->steeringMode = mode;

		if (mode != SteeringMode::NoMode && mode != SteeringMode::Freeze) {
			state->lastSteeringModeChange = std::chrono::steady_clock::now();
		} else {
			fireSteeringModeChanged(mode);
		}

		return true;
	});
}

auto KinovaArm::doHasFailed(std::promise<bool> token) const -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable { token.set_value(state->isInFailState); });
}

auto KinovaArm::doGetPosition(std::promise<Coordinates> token) const -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable {
		token.set_value(state->currentPosition.value_or(Coordinates{}));
	});
}

auto KinovaArm::doGetSteeringMode(std::promise<std::optional<SteeringMode>> token) const -> void {
	asio::dispatch(actionStrand, [this, token = std::move(token)]() mutable { token.set_value(state->steeringMode); });
}

} // namespace KinovaZED::Hw