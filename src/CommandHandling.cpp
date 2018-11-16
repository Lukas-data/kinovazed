#include <stdio.h>
#include <stdexcept>
#include <unistd.h>


#include "CommandHandling.h"

/*Initiailizes Communication with Jaco-Arm and TCPServer.*/
void CommandHandling::init() {

  //Communication to Kinova robotic arm
  printf("Connecting to Jaco arm: ");
  while (true) {
    if( JacoZED.connect() ) {
      printf("Connection established.\n");
      break;
    }
    else {
      
      printf("Retry...\n");
    }
    usleep(1000);
  }
  printf("-------------------------\n");

  //TCP-Server for Connection to RoboRio.
  bool serverCreated = false;
  printf("Connecting to RoboRio...\n");
  while (serverCreated == false) {
    serverCreated = true;
    try {
      //RoboRio.connect();
    }
    catch( const std::runtime_error& e ) {
      printf("Connection Unsuccessfull. Retry...\n");
      serverCreated = false;
    }
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
  printf("Command sent to Roborio\n");
}



//* Debug use only! */
void CommandHandling::debugSendEvent(KinovaFSM::Event e) {
  KinovaSM.sendEvent(e);
}

