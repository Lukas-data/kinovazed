#include <stdio.h>
#include <stdexcept>
#include <unistd.h>


#include "CommandHandling.h"

/*Initiailizes Communication with Jaco-Arm and TCPServer.*/
void CommandHandling::init() {

  //Communication to Kinova robotic arm
  bool ArmConnected = false;
  printf("Connecting to Jaco...\n");
  while (ArmConnected == false) {
      ArmConnected = true;
      try{
        JacoZED.init();
      }
      catch( const std::runtime_error& e ) {
        printf("Connecting with Jaco failed. Retry...\n");
        ArmConnected = false;
      }
      usleep(1000);
    }

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

  //Initialize StateMachine.
  KinovaSM.init(&JacoZED);
}

void CommandHandling::process() {
   printf("CommandHandling::process()\n");
  //Send Command
  //sendCommand(commandOut);
  
}



void CommandHandling::sendCommand(Command::Name command) {
  printf("Command sent to Roborio\n");
}

//* Debug use only! */
void CommandHandling::sendEvent(KinovaFSM::Event e) {
  KinovaSM.sendEvent(e);
}
