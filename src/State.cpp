#include <stdio.h>

#include "State.h"

KinovaArm* State::JacoZED = NULL;

int State::getId() { return  Id; }
void State::init(KinovaArm* jacoZED) {JacoZED = jacoZED; }

// PowerOff
void StatePowerOff::entryAction() {
  printf("Executing StatePowerOff entryAction. JacoZed Points to: %p\n", JacoZED);
}
void StatePowerOff::exitAction() {
  printf("Executing StatePowerOff exitAction.\n");
}
void StatePowerOff::tickAction() {
  printf("Executing StatePowerOff tickAction.\n");
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







