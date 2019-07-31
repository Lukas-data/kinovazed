#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "Event.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"
#include "kindrv.h"

constexpr auto positionRange = 0.05;
constexpr auto rotationRange = 0.25;
constexpr auto velocityRange = 0.000002;

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

	bool Connected = false;
	bool Error = false;
	bool Initialized = false;
	bool Active = false;
	bool Homed = false;
	KinovaStatus::SteeringMode Mode = KinovaStatus::NoMode;
	int currentPosition = -1;

	KinovaPts::Objective TargetObjective;
	KinovaPts::Objective TeachTarget;
	bool NeedsInit;

	int JoystickX = 0;
	int JoystickY = 0;
	int JoystickZ = 0;
	float JoystickCalcFactor = 0.0025;

	timespec ModeChangeTimerStart;
	int ModeChangeTimer;

	timespec MoveTimerStart;

	KinovaFSM::Event ExternalEvent = KinovaFSM::NoEvent;
	KinovaFSM::Event InternalEvent = KinovaFSM::NoEvent;

	float LastCoordinates[6];

	int PointReachedCount;

	void error(const char *funcName, KinDrv::KinDrvException &e, bool warning);
	void error(const char *funcName, const char *errorMsg);
	auto checkIfReached(float *targetCoordinates, float *currentCoordinates) -> bool;
	auto checkCurrents() -> bool;
	void getForces();
};

#endif
