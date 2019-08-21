
#include "State.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"
#include "Log.h"

#include <cstdio>
#include <memory>

std::shared_ptr<KinovaArm> State::JacoZED = nullptr;

void State::entryAction() {
	ALL_LOG(logINFO) << "Entering State " << name;
	entryActionHook();
}

void State::exitAction() {
	ALL_LOG(logINFO) << "Exiting State " << name;
	exitActionHook();
}

void State::tickAction() {
}

void State::init(std::shared_ptr<KinovaArm> jacoZed) {
	JacoZED = jacoZed;
}
void State::setEventVar(int eventVar) {
	eventVariable = eventVar;
}

// PowerOff
void StatePowerOff::entryActionHook() {
	JacoZED->setInactive();
}

// EmergencyStop
void StateEStop::entryActionHook() {
	JacoZED->releaseControl();
	JacoZED->setInactive();
}

void StateEStop::tickAction() {
	JacoZED->reconnectOnError();
}

// Initialize
void StateInitialize::entryActionHook() {
	JacoZED->takeControl();
	JacoZED->checkInitialize();
}
void StateInitialize::exitActionHook() {
	JacoZED->setActive();
}
void StateInitialize::tickAction() {
	JacoZED->initialize();
}

// Retracted
void StateRetract::entryActionHook() {
	JacoZED->startRetracting(false);
}
void StateRetract::tickAction() {
	JacoZED->retract(false);
}

// Unfolding
void StateUnfold::exitActionHook() {
	JacoZED->dontMove();
}
void StateUnfold::tickAction() {
	JacoZED->unfold();
}

// Idle
void StateIdle::entryActionHook() {
	JacoZED->dontMove();
	JacoZED->setInactive();

}
void StateIdle::exitActionHook() {
	JacoZED->setActive();
}

// ChangeMode
void StateChangeMode::entryActionHook() {
	JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(eventVariable));
}
void StateChangeMode::tickAction() {
	JacoZED->modeChangeTimer();
}

// ChangeModeDefault
void StateChangeModeDefault::entryActionHook() {
	JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(0));
}
void StateChangeModeDefault::tickAction() {
	JacoZED->modeChangeTimer();
}

// Steering
void StateSteering::exitActionHook() {
	JacoZED->dontMove();
}
void StateSteering::tickAction() {
	JacoZED->move();
}

// MovePosition
void StateMovePosition::entryActionHook() {
	JacoZED->setTarget(static_cast<Kinova::Objective>(eventVariable));
}
void StateMovePosition::exitActionHook() {
	JacoZED->sequenceDone();
	JacoZED->dontMove();
}
void StateMovePosition::tickAction() {
	JacoZED->moveToPosition(false);
}

//Teach
void StateTeach::entryActionHook() {
	JacoZED->teachPosition(static_cast<Kinova::Objective>(eventVariable));
}
void StateTeach::exitActionHook() {
	JacoZED->dontMove();
}
void StateTeach::tickAction() {
	JacoZED->move();
}

// ChangeModeTeach
void StateChangeModeTeach::entryActionHook() {
	JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(eventVariable));
}
void StateChangeModeTeach::tickAction() {
	JacoZED->modeChangeTimer();
}

// TeachMovePoint
void StateTeachMovePoint::exitActionHook() {
	JacoZED->dontMove();
}
void StateTeachMovePoint::tickAction() {
	JacoZED->moveToPoint();
}

// TeachMoveOrigin
void StateTeachMoveOrigin::exitActionHook() {
	JacoZED->dontMove();
}
void StateTeachMoveOrigin::tickAction() {
	JacoZED->moveToOrigin();
}

// TeachSavePoint
void StateTeachSavePoint::entryActionHook() {
	JacoZED->savePoint(eventVariable);
}

// TeachSaveOrigin
void StateTeachSaveOrigin::entryActionHook() {
	JacoZED->saveOrigin();
}

// TeachDeletePoint
void StateTeachDeletePoint::entryActionHook() {
	JacoZED->deletePoint();
}

// TeachPrevious
void StateTeachPrevious::entryActionHook() {
	JacoZED->previousPoint(eventVariable);
}

// TeachNext
void StateTeachNext::entryActionHook() {
	JacoZED->nextPoint(eventVariable);
}

