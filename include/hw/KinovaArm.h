#ifndef INCLUDE_HW_KINOVA_ARM_H_
#define INCLUDE_HW_KINOVA_ARM_H_

#include "hw/Actor.h"
#include "hw/Coordinates.h"
#include "hw/KinovaStatus.h"
#include "support/Logging.h"

#include "libkindrv/kindrv.h"

#include <chrono>
#include <future>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

namespace KinovaZED::Hw {

struct KinovaArm : Actor {
	explicit KinovaArm(Logger logger);
	KinovaArm(Coordinates homePosition, Logger logger);
	~KinovaArm();

	auto connect() -> bool;
	auto disconnect() -> void;

	auto takeControl() -> bool;
	auto releaseControl() -> bool;

	auto stopMoving() -> bool;
	auto home() -> void;
	auto retract() -> void;
	auto moveTo(Coordinates position) -> void;
	auto setJoystick(int x, int y, int z) -> void;

	auto setSteeringMode(SteeringMode mode) -> bool;

	auto hasFailed() const -> bool;

  private:
	enum struct MovementStatus {
		HomingToHardwareHome,
		HomingToSoftwareHome,
		MovingToPosition,
		Retracting,
	};

	enum struct RetractionStatus {
		Initializing,
		Retracting,
		Retracted,
		Homing,
		Homed,
	};

	template<typename... Args>
	auto logInfo(std::string function, std::string format, Args &&... args) {
		auto prefix = "KinovaArm::" + function + ": ";
		logger->info(prefix + format, std::forward<Args &&>(args)...);
	}

	template<typename... Args>
	auto logError(std::string function, std::string format, Args &&... args) {
		auto prefix = "KinovaArm::" + function + ": ";
		logger->error(prefix + format, std::forward<Args &&>(args)...);
		isInFailState = true;
	}

	template<typename... Args>
	auto logDebug(std::string function, std::string format, Args &&... args) {
		auto prefix = "KinovaArm::" + function + ": ";
		logger->debug(prefix + format, std::forward<Args &&>(args)...);
	}

	template<typename... Args>
	auto logWarning(std::string function, std::string format, Args &&... args) {
		auto prefix = "KinovaArm::" + function + ": ";
		logger->warn(prefix + format, std::forward<Args &&>(args)...);
	}

	auto initialize() -> void;

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

	std::optional<Coordinates> const homePosition;
	Logger logger;

	std::recursive_mutex accessLock{};

	std::atomic_bool runUpdateLoop{};
	std::future<void> updateLoopHandle{};

	std::optional<KinDrv::JacoArm> arm{};

	std::optional<Coordinates> currentPosition{};
	std::optional<Coordinates> targetPosition{};

	std::optional<RetractionMode> retractionMode{};

	std::optional<SteeringMode> steeringMode{};
	std::optional<std::chrono::steady_clock::time_point> lastSteeringModeChange{};

	float fullCurrent{};

	std::optional<MovementStatus> movementStatus;
	std::optional<RetractionStatus> retractionStatus;

	bool hasControl{};
	bool wasHomed{};
	bool isInFailState{};
};

} // namespace KinovaZED::Hw

#endif
