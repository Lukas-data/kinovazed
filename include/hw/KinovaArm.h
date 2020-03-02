#ifndef INCLUDE_HW_KINOVA_ARM_H_
#define INCLUDE_HW_KINOVA_ARM_H_

#include "hw/Actor.h"
#include "hw/Coordinates.h"
#include "support/Logging.h"

#include "libkindrv/kindrv.h"

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

	auto connect() -> bool;
	auto disconnect() -> void;

	auto takeControl() -> bool;
	auto releaseControl() -> bool;
	auto initialize() -> void;

	auto stopMoving() -> bool;
	auto home() -> void;
	auto retract() -> void;
	auto moveTo(Coordinates position) -> void;
	auto setJoystick(int x, int y, int z) -> void;

	auto setSteeringMode(SteeringMode mode) -> bool;

	auto getPosition() const -> Coordinates;
	auto getSteeringMode() const -> std::optional<SteeringMode>;

	auto hasFailed() const -> bool;

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

	template<typename... Args>
	auto logError(std::string function, std::string format, Args &&... args) {
		LoggingMixin::template logError(function, format, std::forward<Args>(args)...);
		state->isInFailState = true;
	}

	auto startUpdateLoop() -> void;
	auto stopUpdateLoop() -> void;

	auto updatePosition() -> void;
	auto readPosition() -> Coordinates;

	auto updateRetractionMode() -> void;
	auto readRetractionMode() -> RetractionMode;

	auto updateSteeringMode() -> void;

	auto checkCurrents() -> void;

	auto reconnectOnError() -> void;

	auto moveToHardwareHome() -> void;
	auto moveToSoftwareHome() -> void;
	auto moveToRetractionPoint() -> void;

	auto canChangeMode() -> bool;

	auto pushButton(int index) -> void;
	auto releaseJoystick() -> void;

	std::recursive_mutex accessLock;
	std::optional<KinDrv::JacoArm> arm;
	std::optional<Coordinates> const homePosition;
	std::optional<VolatileState> state;

	std::atomic_bool runUpdateLoop;
	std::future<void> updateLoopHandle;
};

} // namespace KinovaZED::Hw

#endif
