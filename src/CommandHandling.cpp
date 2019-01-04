#include <stdio.h>
#include <stdexcept>
#include <unistd.h>
#include "Log.h"


#include "CommandHandling.h"

/*Initiailizes Communication with Jaco-Arm and TCPServer.*/
void CommandHandling::init() {
  //Connect to Kinova robotic arm
  ALL_LOG(logDEBUG3) << "Trying to connect to JacoArm.";
  while (true) {
    if( JacoZED.connect() ) {
      break;
    }
    else { ALL_LOG(logDEBUG1) << "Connection to JacoArm unsuccessfull. Retry."; }
    usleep(1000000);
  }

  //Connect to RoboRio (ZED Main Controller)
  ALL_LOG(logDEBUG3) << "Trying to connect to RoboRio";
  while (true) {
    if ( RoboRio.connect() ) {
      ALL_LOG(logINFO) << "Connection to RoboRio established.";
      break;
    }
    else { ALL_LOG(logDEBUG1) << "Connection to RoboRio unsuccessfull. Retry."; }
    usleep(1000000);
  }

  //Initialize StateMachine.
  KinovaSM.init(&JacoZED);
}

void CommandHandling::process() {
  KinovaFSM::Event newEvent = KinovaFSM::NoEvent;
  int newVar = 0;

  KinovaFSM::Event oldCommand = CommandIn;
  int oldCommandVar = CommandVarIn;
  getInputs();

  if (CommandIn != oldCommand || CommandVarIn != oldCommandVar) {
    ALL_LOG(logDEBUG) << "CommandHandling: Recieved Event '"
                      << KinovaFSM::EventName[CommandIn] 
                      << ":" << CommandVarIn << "'" ;
  }
  //Check for E_Stop
  if ( CommandIn == KinovaFSM::E_Stop )
  { 
    newEvent = KinovaFSM::E_Stop;
    newVar = 0;
  }
  else {
    //get Hardware Error
    if( JacoZED.getError() ) {
      newEvent = KinovaFSM::E_Stop;
    }
    //get Internal HW Events
    else {
      newEvent = JacoZED.getInternalEvent();
    }
    if (newEvent == KinovaFSM::NoEvent) {
      //get Events from RoboRio
      newEvent = CommandIn;
      newVar = CommandVarIn;
    }
  }
  checkInputEvent(newEvent, newVar);
  KinovaSM.sendEvent(newEvent,newVar);
  bool processed = KinovaSM.process();

  oldCommand = CommandOut;
  oldCommandVar = CommandVarOut;

  KinovaFSM::Event HWEvent = JacoZED.getExternalEvent();
  int HWVar = 0;
  bool enforce = getHWEventVar(HWEvent, HWVar);
  if ( HWEvent == KinovaFSM::NoEvent ) {
    if ( processed ) {
      //Processed Event
      CommandOut = newEvent;
      CommandVarOut = newVar;
    }
    //Don't repeat last Event when HW-Event recieved
    if (newEvent > KinovaFSM::numberOfNonHWEvents) {
      CommandOut = KinovaFSM::NoEvent;
      CommandVarOut = 0; 
    }
  }
  else if (HWEvent == newEvent) {
    CommandOut = KinovaFSM::NoEvent;
    CommandVarOut = 0; 
    
  }
  else {
    CommandOut = HWEvent;
    CommandVarOut = HWVar;
  }
  sendOutputs(CommandOut, CommandVarOut);
  if (CommandOut != oldCommand || CommandVarOut != oldCommandVar) {
    ALL_LOG(logDEBUG) << "CommandHandling: Sent Event '" 
                      << KinovaFSM::EventName[CommandOut] 
                      << ":" << CommandVarOut << "'" ;
  }
}


/*Recieves Inputs from TCP Connection with Roborio connection. Writes Joystick Data directly to hardware.*/
void CommandHandling::getInputs() {
  CommandIn = static_cast<KinovaFSM::Event>(RoboRio.getCommand());
  CommandVarIn = RoboRio.getData(0);
  bool JSisZero = true;
  for(int n = 0; n<3; n++) {
    DataIn[n] = RoboRio.getData(n+1);
    if (DataIn[n] != 0) {
      JSisZero = false;
    }
  }
  JacoZED.setJoystick(DataIn[0],DataIn[1],DataIn[2]);
  //Event is MoveJoystick, if Joystick moves and No Event is set.
  if (!JSisZero && CommandIn !=  KinovaFSM::E_Stop) {
    CommandIn = KinovaFSM::MoveJoystick;
  }
}


/*Sends Output Variables on TCP*/
void CommandHandling::sendOutputs(int event, int eventVar) {
  RoboRio.sendTCP(event,eventVar,0,0,0);
}



/*define Exception for Events, that should or should not be sent to Statemachine*/
void CommandHandling::checkInputEvent(KinovaFSM::Event &event, int &eventVar) {
  KinovaFSM::Event eventToCheck = event;
  int eventVarToCheck = eventVar;

  // 'SetMode' is only sent when mode is not allready set.
  int currentMode = JacoZED.getMode();
  if (JacoZED.getActive() && eventToCheck == KinovaFSM::SetMode && eventVarToCheck == currentMode ) {
    //printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
    event = KinovaFSM::NoEvent;
    eventVar = 0;
  }

  //'GoToPosition' is only sent when the Arm is not allready at the Position
  int currentPosition = JacoZED.getCurrentPosition();
  if (JacoZED.getActive() && eventToCheck == KinovaFSM::GoToPosition && eventVarToCheck == currentPosition ) {
    //printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
    event = KinovaFSM::NoEvent;
    eventVar = 0;
  }



  
}

/*retrieves EventVariable for differen HW-Events.*/
bool CommandHandling::getHWEventVar(KinovaFSM::Event &event, int &eventVar) {
  //Hardware Events
  switch (event) {
    case KinovaFSM::ModeSet             : eventVar = JacoZED.getMode();
                                          return true;
    case KinovaFSM::SequenceDone        : eventVar = JacoZED.getCurrentPosition();
                                          return true;
    case KinovaFSM::PointReached        :
    case KinovaFSM::PointSaved          : 
    case KinovaFSM::PointNotSaved       :
    case KinovaFSM::PreviousPointNotSet : 
    case KinovaFSM::PreviousPointSet    :
    case KinovaFSM::NextPointNotSet     : 
    case KinovaFSM::NextPointSet        : eventVar = JacoZED.getCurrentPoint();
                                          return true;
  }
  return false;
  
}


/************************************/
//* Debug use only! */
void CommandHandling::debugSendEvent(KinovaFSM::Event e) {
  KinovaSM.sendEvent(e);
}

void CommandHandling::debugSendEvent(KinovaFSM::Event e, int var) {
  KinovaSM.sendEvent(e, var);
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

