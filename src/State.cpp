#include <stdio.h>


#include "State.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"
#include "Log.h"

KinovaArm* State::JacoZED = NULL;

void State::init(KinovaArm* jacoZED) {JacoZED = jacoZED; }
void State::setEventVar(int eventVar) { EventVariable = eventVar; }

// PowerOff
void StatePowerOff::entryAction() {
  ALL_LOG(logINFO) << "Entering State PowerOff";
  JacoZED->setInactive();
}
void StatePowerOff::exitAction() {
  ALL_LOG(logINFO) << "Exiting State PowerOff";
}
void StatePowerOff::tickAction() {
}

// EmergencyStop
void StateEStop::entryAction() {
  ALL_LOG(logINFO) << "Entering State EStop";
  JacoZED->releaseControl();
  JacoZED->setInactive();
}
void StateEStop::exitAction() {
  ALL_LOG(logINFO) << "Exiting State EStop";
}
void StateEStop::tickAction() {
}

// Initialize
void StateInitialize::entryAction() {
  ALL_LOG(logINFO) << "Entering State Initialize";
  JacoZED->takeControl();
  JacoZED->checkInitialize();
}
void StateInitialize::exitAction() {
  ALL_LOG(logINFO) << "Exiting State Initialize ";
  JacoZED->setActive();
}
void StateInitialize::tickAction() {
  JacoZED->initialize();
}

// Idle
void StateIdle::entryAction() {
  ALL_LOG(logINFO) << "Entering State Idle";
  JacoZED->dontMove();
  JacoZED->setInactive();
  
}
void StateIdle::exitAction() {
  ALL_LOG(logINFO) << "Exiting State Idle";
  JacoZED->setActive();
}
void StateIdle::tickAction() {
}

// ChangeMode
void StateChangeMode::entryAction() {
  ALL_LOG(logINFO) << "Entering State ChangeMode";
  JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(EventVariable));
}
void StateChangeMode::exitAction() {
 ALL_LOG(logINFO) << "Exiting State ChangeMode";
}
void StateChangeMode::tickAction() {
  JacoZED->modeChangeTimer();
}

// Steering
void StateSteering::entryAction() {
  ALL_LOG(logINFO) << "Entering State Steering";
}
void StateSteering::exitAction() {
  ALL_LOG(logINFO) << "Exiting State Steering";
  JacoZED->dontMove();
}
void StateSteering::tickAction() {
  JacoZED->move();
}

// MovePosition
void StateMovePosition::entryAction() {
  ALL_LOG(logINFO) << "Entering State MovePosition";
  JacoZED->setTarget(static_cast<KinovaPts::Objective>(EventVariable));
}
void StateMovePosition::exitAction() {
  ALL_LOG(logINFO) << "Exiting State MovePosition";
  JacoZED->sequenceDone();
  JacoZED->dontMove();
}
void StateMovePosition::tickAction() {
  JacoZED->moveToPosition(false);
}

//Teach
void StateTeach::entryAction() {
  ALL_LOG(logINFO) << "Entering State Teach";
  JacoZED->teachPosition(static_cast<KinovaPts::Objective>(EventVariable));
}
void StateTeach::exitAction() {
  ALL_LOG(logINFO) << "Exiting State Teach";
  JacoZED->dontMove();
}
void StateTeach::tickAction() {
  JacoZED->move();
}

// ChangeModeTeach
void StateChangeModeTeach::entryAction() {
  ALL_LOG(logINFO) << "Entering State ChangeModeTeach";
  JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(EventVariable));
}
void StateChangeModeTeach::exitAction() {
  ALL_LOG(logINFO) << "Exiting State ChangeModeTeach";
}
void StateChangeModeTeach::tickAction() {
  JacoZED->modeChangeTimer();
}

// TeachMovePoint
void StateTeachMovePoint::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachMovePoint";
  
}
void StateTeachMovePoint::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachMovePoint";
  JacoZED->dontMove();
}
void StateTeachMovePoint::tickAction() {
  JacoZED->moveToPoint();
}

// TeachMoveOrigin
void StateTeachMoveOrigin::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachMoveOrigin";
  
}
void StateTeachMoveOrigin::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachMoveOrigin";
  JacoZED->dontMove();
}
void StateTeachMoveOrigin::tickAction() {
  JacoZED->moveToOrigin();
}

// TeachSavePoint
void StateTeachSavePoint::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachSavePoint";
  JacoZED->savePoint(EventVariable); 
}
void StateTeachSavePoint::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachSavePoint";
}
void StateTeachSavePoint::tickAction() {
}

// TeachSaveOrigin
void StateTeachSaveOrigin::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachSaveOrigin";
  JacoZED->saveOrigin(); 
}
void StateTeachSaveOrigin::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachSaveOrigin";
}
void StateTeachSaveOrigin::tickAction() {
}

// TeachPrevious
void StateTeachPrevious::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachPrevious";
  JacoZED->previousPoint(EventVariable); 
}
void StateTeachPrevious::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachPrevious";
}
void StateTeachPrevious::tickAction() {
}


// TeachNext
void StateTeachNext::entryAction() {
  ALL_LOG(logINFO) << "Entering State TeachNext";
  JacoZED->nextPoint(EventVariable); 
}
void StateTeachNext::exitAction() {
  ALL_LOG(logINFO) << "Exiting State TeachNext";
}
void StateTeachNext::tickAction() {
}

