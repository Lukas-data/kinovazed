#include <stdio.h>


#include "State.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"

KinovaArm* State::JacoZED = NULL;

void State::init(KinovaArm* jacoZED) {JacoZED = jacoZED; }
void State::setEventVar(int eventVar) { EventVariable = eventVar; }

// PowerOff
void StatePowerOff::entryAction() {
  printf("Executing StatePowerOff entryAction.\n");
  JacoZED->releaseControl();  
}
void StatePowerOff::exitAction() {
  printf("Executing StatePowerOff exitAction.\n");
}
void StatePowerOff::tickAction() {
}

// EmergencyStop
void StateEStop::entryAction() {
  printf("Executing StateEStop entryAction.\n");
}
void StateEStop::exitAction() {
  printf("Executing StateEStop exitAction.\n");
}
void StateEStop::tickAction() {
}

// Initialize
void StateInitialize::entryAction() {
  printf("Executing StateInitialize entryAction.\n");
  JacoZED->takeControl();
  JacoZED->checkInitialize();
}
void StateInitialize::exitAction() {
  printf("Executing StateInitialize exitAction.\n");
}
void StateInitialize::tickAction() {
  JacoZED->initialize();
}

// Idle
void StateIdle::entryAction() {
  printf("Executing StateIdle entryAction.\n");
  JacoZED->dontMove();
  
}
void StateIdle::exitAction() {
  printf("Executing StateIdle exitAction.\n");
}
void StateIdle::tickAction() {
}

// ChangeMode
void StateChangeMode::entryAction() {
  printf("Executing StateChangeMode entryAction.\n");
  JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(EventVariable));
}
void StateChangeMode::exitAction() {
  printf("Executing StateChangeMode exitAction.\n");
}
void StateChangeMode::tickAction() {
  JacoZED->modeChangeTimer();
}

// Steering
void StateSteering::entryAction() {
  printf("Executing StateSteering entryAction.\n");
}
void StateSteering::exitAction() {
  printf("Executing StateSteering exitAction.\n");
  JacoZED->dontMove();
}
void StateSteering::tickAction() {
  JacoZED->move();
}

// MovePosition
void StateMovePosition::entryAction() {
  printf("Executing StateMovePosition entryAction.\n");
  JacoZED->setTarget(static_cast<KinovaPts::Objective>(EventVariable));
}
void StateMovePosition::exitAction() {
  printf("Executing StateMovePosition exitAction.\n");
}
void StateMovePosition::tickAction() {
  JacoZED->moveToPosition();
}

//Teach
void StateTeach::entryAction() {
  printf("Executing StateTeach entryAction.\n");
  JacoZED->teachPosition(static_cast<KinovaPts::Objective>(EventVariable));
}
void StateTeach::exitAction() {
  printf("Executing StateTeach exitAction.\n");
  JacoZED->dontMove();
}
void StateTeach::tickAction() {
  JacoZED->move();
}

// ChangeModeTeach
void StateChangeModeTeach::entryAction() {
  printf("Executing StateChangeModeTeach entryAction.\n");
  JacoZED->changeMode(static_cast<KinovaStatus::SteeringMode>(EventVariable));
}
void StateChangeModeTeach::exitAction() {
  printf("Executing StateChangeModeTeach exitAction.\n");
}
void StateChangeModeTeach::tickAction() {
  JacoZED->modeChangeTimer();
}

// TeachMovePoint
void StateTeachMovePoint::entryAction() {
  printf("Executing StateTeachMovePoint entryAction.\n");
  JacoZED->setTarget(static_cast<KinovaPts::Objective>(EventVariable));
  
}
void StateTeachMovePoint::exitAction() {
  printf("Executing StateTeachMovePoint exitAction.\n");
}
void StateTeachMovePoint::tickAction() {
  JacoZED->moveToPoint();
}

// TeachSavePoint
void StateTeachSavePoint::entryAction() {
  printf("Executing StateTeachSavePoint entryAction.\n");
  JacoZED->savePoint(); 
}
void StateTeachSavePoint::exitAction() {
  printf("Executing StateTeachSavePoint exitAction.\n");
}
void StateTeachSavePoint::tickAction() {
}

// TeachNext
void StateTeachNext::entryAction() {
  printf("Executing StateTeachNext entryAction.\n");
  JacoZED->nextPoint(); 
}
void StateTeachNext::exitAction() {
  printf("Executing StateTeachNext exitAction.\n");
}
void StateTeachNext::tickAction() {
}

