#include <stdio.h>

#include "State.h"

KinovaArm* State::JacoZED = NULL;

int State::getId() { return  Id; }
void State::init(KinovaArm* jacoZED) {JacoZED = jacoZED; }

// PowerOff
void StatePowerOff::entryAction() {
  printf("Executing StatePowerOff entryAction.\n");
  
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
  //Don't Move!
}
void StateInitialize::tickAction() {
  printf("Executing StateInitialize tickAction.\n");
  JacoZED->initialize();
}

// Steering
void StateSteering::entryAction() {
  printf("Executing StateSteering entryAction.\n");
}
void StateSteering::exitAction() {
  printf("Executing StateSteering exitAction.\n");
}
void StateSteering::tickAction() {
  printf("Executing StateSteering tickAction.\n");
}

// ChangeMode
void StateChangeMode::entryAction() {
  printf("Executing StateChangeMode entryAction.\n");
}
void StateChangeMode::exitAction() {
  printf("Executing StateChangeMode exitAction.\n");
}
void StateChangeMode::tickAction() {
  printf("Executing StateChangeMode tickAction.\n");
}

// MoveToPos
void StateMoveToPos::entryAction() {
  printf("Executing StateMoveToPos entryAction.\n");
}
void StateMoveToPos::exitAction() {
  printf("Executing StateMoveToPos exitAction.\n");
}
void StateMoveToPos::tickAction() {
  printf("Executing StateMoveToPos tickAction.\n");
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









