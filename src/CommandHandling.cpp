#include <stdio.h>
#include <stdexcept>
#include <unistd.h>


#include "CommandHandling.h"

/*Initiailizes Communication with Jaco-Arm and TCPServer.*/
void CommandHandling::init() {
  //Connect to Kinova robotic arm
  printf("Connecting to Jaco arm: ");
  while (true) {
    if( JacoZED.connect() ) {
      printf("Connection established.\n");
      break;
    }
    else { printf("Retry...\n"); }
    usleep(1000);
  }
  printf("-------------------------\n");

  //Connect to RoboRio (ZED Main Controller)
  printf("Connecting to RoboRio: ");
  while (true) {
    if ( RoboRio.connect() ) {
      printf("Connection established.\n");
      break;
    }
    else { printf("Retry...\n"); }
    usleep(1000);
  }
  printf("-------------------------\n");

  //Initialize StateMachine.
  KinovaSM.init(&JacoZED);
}

void CommandHandling::process() {
  KinovaFSM::Event newEvent = KinovaFSM::NoEvent;
  //Send Command
  //JacoZED.
  //sendCommand(commandOut);
  KinovaSM.process();
  //getEvents
  if( JacoZED.getError() ) {
    newEvent = KinovaFSM::Error;
  }
  else {
    newEvent = JacoZED.getEvent();
  }
  if (newEvent == KinovaFSM::NoEvent)
  { 
    printf("No Hardware Events\n");
  }
  KinovaSM.sendEvent(newEvent);
}

void CommandHandling::sendCommand(Command::Name command) {
  printf("TODO: Command sent to Roborio\n");
}



//* Debug use only! */
void CommandHandling::debugSendEvent(KinovaFSM::Event e) {
  KinovaSM.sendEvent(e);
}

void CommandHandling::debugSetJoystick(int x, int y, int z) {
  //printf("CommandHandling::setJoystick (%d,%d,%d)\n",x,y,z);
  JacoZED.setJoystick(x,y,z);
}

void CommandHandling::debugPrintPosition() {
  float coordinates[6];
  JacoZED.getPosition(coordinates);
  printf("Current Position = (");
  for (int i = 0; i<6; i++) {
    printf("%f",coordinates[i]);
    if( i<5 ) { printf(", "); }
  }
  printf(").\n");
}

