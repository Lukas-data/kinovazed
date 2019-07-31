#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "Event.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"
#include "kindrv.h"

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

	void teachPosition(KinovaPts::Objective targetObjective);
	void moveToPoint();
	void moveToOrigin();
	void savePoint(int EventVariable);
	void saveOrigin();
	void deletePoint();
	void previousPoint(int EventVariable);
	void nextPoint(int EventVariable);

	void setJoystick(int x, int y, int z);

	void setTarget(KinovaPts::Objective targetObjective);

	auto getError() -> bool;
	auto getInitialize() -> bool;
	auto getActive() -> bool;
	auto getMode() -> int;
	auto getCurrentPosition() -> int;
	auto getCurrentPoint() -> int;

	auto getExternalEvent() -> KinovaFSM::Event;
	auto getInternalEvent() -> KinovaFSM::Event;
	void getPosition(float *coordinates);

	void setActive();
	void setInactive();

private:
	KinDrv::JacoArm *arm;
	PositionHandling PositionHandler{};

	bool connected = false;
	bool Error = false;
	bool initialized = false;
	bool active = false;
	bool homed = false;
	KinovaStatus::SteeringMode mode = KinovaStatus::NoMode;
	int currentPosition = -1;

	KinovaPts::Objective TargetObjective;
	KinovaPts::Objective teachTarget;

	int joystickX = 0;
	int joystickY = 0;
	int joystickZ = 0;

	timespec modeChangeTimerStart;
	int ModeChangeTimer;

	timespec moveTimerStart;

	KinovaFSM::Event externalEvent = KinovaFSM::NoEvent;
	KinovaFSM::Event internalEvent = KinovaFSM::NoEvent;

	float lastCoordinates[6];

	int pointReachedCount;

	void error(const char *funcName, KinDrv::KinDrvException const &e, bool warning);
	void error(const char *funcName, const char *errorMsg);
	auto checkIfReached(float *currentCoordinates) -> bool;
	auto checkCurrents() -> bool;
	void getForces();
};

#endif
