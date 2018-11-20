#include <stdio.h>


#include "State.h"
#include "KinovaStatus.h"

KinovaArm* State::JacoZED = NULL;

void State::init(KinovaArm* jacoZED) {JacoZED = jacoZED; }

// PowerOff
void StatePowerOff::entryAction() {
  printf("Executing StatePowerOff entryAction.\n");
  JacoZED->releaseControl();  
}
void StatePowerOff::exitAction() {
  printf("Executing StatePowerOff exitAction.\n");
}
void StatePowerOff::tickAction() {
  printf("Executing StatePowerOff tickAction.\n");
}

// Initialize
void StateInitialize::entryAction() {
  printf("Executing StateInitialize entryAction.\n");
  JacoZED->takeControl();   
}
void StateInitialize::exitAction() {
  printf("Executing StateInitialize exitAction.\n");
  JacoZED->changeMode(KinovaStatus::Translation);
}
void StateInitialize::tickAction() {
  printf("Executing StateInitialize tickAction.\n");
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
  printf("Executing StateIdle tickAction.\n");
}

// ChangeModeTranslation
void StateChangeModeTranslation::entryAction() {
  printf("Executing StateChangeModeTranslation entryAction.\n");
  JacoZED->changeMode(KinovaStatus::Translation);
}
void StateChangeModeTranslation::exitAction() {
  printf("Executing StateChangeModeTranslation exitAction.\n");
}
void StateChangeModeTranslation::tickAction() {
  printf("Executing StateChangeModeTranslation tickAction.\n");
  JacoZED->modeChangeTimer();
}

// ChangeModeRotation
void StateChangeModeRotation::entryAction() {
  printf("Executing StateChangeModeRotation entryAction.\n");
  JacoZED->changeMode(KinovaStatus::Rotation);
}
void StateChangeModeRotation::exitAction() {
  printf("Executing StateChangeModeRotation exitAction.\n");
}
void StateChangeModeRotation::tickAction() {
  printf("Executing StateChangeModeRotation tickAction.\n");
  JacoZED->modeChangeTimer();
}

// ChangeModeAxis1
void StateChangeModeAxis1::entryAction() {
  printf("Executing StateChangeModeAxis1 entryAction.\n");
  JacoZED->changeMode(KinovaStatus::Axis1);
}
void StateChangeModeAxis1::exitAction() {
  printf("Executing StateChangeModeAxis1 exitAction.\n");
}
void StateChangeModeAxis1::tickAction() {
  printf("Executing StateChangeModeAxis1 tickAction.\n");
  JacoZED->modeChangeTimer();
}

// ChangeModeAxis2
void StateChangeModeAxis2::entryAction() {
  printf("Executing StateChangeModeAxis2 entryAction.\n");
  JacoZED->changeMode(KinovaStatus::Axis2);
}
void StateChangeModeAxis2::exitAction() {
  printf("Executing StateChangeModeAxis2 exitAction.\n");
}
void StateChangeModeAxis2::tickAction() {
  printf("Executing StateChangeModeAxis2 tickAction.\n");
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
  printf("Executing StateSteering tickAction.\n");
  JacoZED->move();
}

// MoveTrajectory
void StateMoveTrajectory::entryAction() {
  printf("Executing StateMoveTrajectory entryAction.\n");
}
void StateMoveTrajectory::exitAction() {
  printf("Executing StateMoveTrajectory exitAction.\n");
}
void StateMoveTrajectory::tickAction() {
  printf("Executing StateMoveTrajectory tickAction.\n");
}

// EmergencyStop
void StateEStop::entryAction() {
  printf("Executing StateEStop entryAction.\n");
}
void StateEStop::exitAction() {
  printf("Executing StateEStop exitAction.\n");
}
void StateEStop::tickAction() {
  printf("Executing StateEStop tickAction.\n");
}









