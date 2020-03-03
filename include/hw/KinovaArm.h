#ifndef INCLUDE_HW_KINOVA_ARM_H_
#define INCLUDE_HW_KINOVA_ARM_H_

#include "hw/Actor.h"
#include "hw/Coordinates.h"
#include "support/Logging.h"

#include "libkindrv/kindrv.h"

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/io_context_strand.hpp>
#include <asio/steady_timer.hpp>

#include <chrono>
#include <future>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

namespace KinovaZED::Hw {


struct KinovaArm : Actor, LoggingMixin {
	explicit KinovaArm(Logger logger);
	KinovaArm(Coordinates homePosition, Logger logger);
	~KinovaArm();

  private:
	enum struct RetractionMode {
		NormalToReady,
		ReadyToStandby,
		ReadyToRetract,
		RetractToStandby,
		RetractToReady,
		Normal,
		NoInitToReady,
		Error = 25000,
	};

	enum struct MovementStatus {
		HomingToHardwareHome,
		HomingToSoftwareHome,
		MovingToPosition,
		Retracting,
		Initializing,
	};

	enum struct RetractionStatus {
		Initializing,
		Retracting,
		Retracted,
		Homing,
		Homed,
	};

	struct VolatileState {
		std::optional<Coordinates> currentPosition{};
		std::optional<Coordinates> targetPosition{};

		std::optional<RetractionMode> retractionMode{};

		std::optional<SteeringMode> steeringMode{};
		std::optional<std::chrono::steady_clock::time_point> lastSteeringModeChange{};

		std::optional<MovementStatus> movementStatus;
		std::optional<RetractionStatus> retractionStatus;

		float fullCurrent{};

		bool hasControl{};
		bool wasHomed{};
		bool isInFailState{};
	};

	KinovaArm(std::optional<Coordinates> origin, Logger logger);

	/// @section Actor Interface Implementation

	auto doConnect(std::promise<bool> token) -> void override;
	auto doDisconnect(std::promise<void> token) -> void override;
	auto doTakeControl(std::promise<bool> token) -> void override;
	auto doReleaseControl(std::promise<bool> token) -> void override;
	auto doInitialize(std::promise<void> token) -> void override;
	auto doStopMoving(std::promise<bool> token) -> void override;
	auto doHome(std::promise<void> token) -> void override;
	auto doRetract(std::promise<void> token) -> void override;
	auto doMoveTo(Coordinates position, std::promise<void> token) -> void override;
	auto doSetJoystick(int x, int y, int z, std::promise<void> token) -> void override;
	auto doSetSteeringMode(SteeringMode mode, std::promise<bool> token) -> void override;
	auto doHasFailed(std::promise<bool> token) const -> void override;
	auto doGetPosition(std::promise<Coordinates> token) const -> void override;
	auto doGetSteeringMode(std::promise<std::optional<SteeringMode>> token) const -> void override;

	/// @section Kinova Arm Implementation

	auto scheduleStateUpdate() -> void;
	auto performStateUpdate() -> void;
	auto stopUpdateLoop() -> void;

	auto checkCurrents() -> void;
	auto checkMovement() -> void;
	auto updatePosition() -> void;
	auto readPosition() -> Coordinates;
	auto handlePosition(Coordinates newPosition) -> void;
	auto updateRetractionMode() -> void;
	auto readRetractionMode() -> RetractionMode;
	auto handleRetractionMode(RetractionMode newMode) -> void;
	auto updateSteeringMode() -> void;

	auto reconnectOnError() -> void;

	auto moveToHardwareHome() -> void;
	auto moveToSoftwareHome() -> void;
	auto moveToRetractionPoint() -> void;

	auto canChangeMode() -> bool;

	auto pushButton(int index) -> void;
	auto releaseJoystick() -> void;
	auto moveJoystick(KinDrv::jaco_joystick_axis_t position) -> void;
	auto eraseTrajectories() -> void;

	template<typename... Args>
	auto logError(std::string function, std::string format, Args &&... args) {
		LoggingMixin::template logError(function, format, std::forward<Args>(args)...);
		state->isInFailState = true;
	}

	asio::io_context ioContext{};
	asio::io_context::strand actionStrand{ioContext};
	asio::steady_timer stateUpdateTimer{actionStrand};
	std::future<void> ioRunner{};
	std::optional<KinDrv::JacoArm> arm{};
	std::optional<VolatileState> state{};

	std::optional<Coordinates> const homePosition;
	asio::executor_work_guard<asio::io_context::executor_type> ioWorkGuard;
};

} // namespace KinovaZED::Hw

#endif
