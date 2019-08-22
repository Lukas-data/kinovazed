#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "Event.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"
#include "kindrv.h"

#include <chrono>

#if __cplusplus >= 201703L
#include <optional>
#else
#include <experimental/optional>
#endif

constexpr auto positionRange = 0.05;
constexpr auto rotationRange = 0.25;
constexpr auto velocityRange = 0.000002;
constexpr auto joystickCalcFactor = 0.0025f;

struct KinovaArm {
	KinovaArm() = default;
	~KinovaArm();

	auto connect() -> bool;
	void disconnect();
	void reconnectOnError();
	void takeControl();
	void releaseControl();

	void dontMove();
	void checkInitialize();
	void initialize();
	void startRetracting(bool init);
	void retract(bool init);
	void unfold();
	void changeMode(KinovaStatus::SteeringMode nextMode);
	void modeChangeTimer();
	void move();
	void moveToPosition(bool init);
	void sequenceDone();

	void teachPosition(Kinova::Objective targetObjective);
	void moveToPoint();
	void moveToOrigin();
	void savePoint(int EventVariable);
	void saveOrigin();
	void deletePoint();
	void previousPoint(int EventVariable);
	void nextPoint(int EventVariable);

	void setJoystick(int x, int y, int z);

	void setTarget(Kinova::Objective targetObjective);

	auto getError() -> bool;
	auto getInitialize() -> bool;
	auto getActive() -> bool;
	auto getMode() -> int;
	auto getCurrentPosition() -> int;
	auto getCurrentPoint() -> int;
	auto getCurrentPoint(Kinova::Objective target) -> int;

	auto getExternalEvent() -> KinovaFSM::Event;
	auto getInternalEvent() -> KinovaFSM::Event;
	void getPosition(float *coordinates);

	void setActive();
	void setInactive();

private:
	KinDrv::JacoArm *arm { nullptr };
	PositionHandling PositionHandler{};

	bool connected = false;
	bool Error = false;
	bool initialized = false;
	bool active = false;
	bool homed = false;
	KinovaStatus::SteeringMode mode = KinovaStatus::NoMode;
	int currentPosition = -1;

	Kinova::Objective TargetObjective { Kinova::Objective::NoObjective };
	Kinova::Objective teachTarget { Kinova::Objective::NoObjective };

	int joystickX = 0;
	int joystickY = 0;
	int joystickZ = 0;

	std::chrono::milliseconds maxModeChangeTimer { };

#if __cplusplus >= 201703L
	std::optional<std::chrono::time_point<std::chrono::steady_clock>> modeChangeTimerStart { std::nullopt };
	std::optional<std::chrono::time_point<std::chrono::steady_clock>> moveTimerStart { std::nullopt };
#else
	std::experimental::optional<std::chrono::time_point<std::chrono::steady_clock>> modeChangeTimerStart { std::experimental::nullopt };
	std::experimental::optional<std::chrono::time_point<std::chrono::steady_clock>> moveTimerStart { std::experimental::nullopt };
#endif

	KinovaFSM::Event externalEvent = KinovaFSM::NoEvent;
	KinovaFSM::Event internalEvent = KinovaFSM::NoEvent;

	float lastCoordinates[6] { };

	int pointReachedCount { };

	void error(const char *funcName, KinDrv::KinDrvException const &e, bool warning);
	void error(const char *funcName, const char *errorMsg);
	auto checkIfReached(float *currentCoordinates) -> bool;
	auto checkCurrents() -> bool;
	void getForces();
};

#endif
