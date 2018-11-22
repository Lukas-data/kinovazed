
/***************************************************************************
 *  Kinova Main
 *
 *  Created: Fri Jun 15 2018
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <poll.h>
//#include "StateMachine.h"
//#include "Transition.h"
#include "CommandHandling.h"

//StateMachine *fsm;
//CommandHandling *communication;

CommandHandling communication;

void wait(int n) {
  for (int i =0; i<n; i++) {
    communication.process();
    usleep(200000);
  }
}

void move(int x, int y, int z, int n) {
  printf("Move araound\n");
  for (int i =0; i<n; i++) {
    communication.debugSetJoystick(x,y,z);
    communication.process();
    usleep(200000);
  }
  communication.debugSetJoystick(0,0,0);
  communication.process();
  usleep(200000);
}


int main(int argc, char *argv[])
{

  printf("-------------------------\n");
  printf("|      KinovaTest!      |\n");
  printf("-------------------------\n");

  /************************/
  /* Testing StateMachine */
  /************************/
  /*
  StateMachine fsm;
  fsm.init();
  int i;
  printf("0 = No Event\n1 = Initialize\n2=Shutdown\n");
  while (true) {
    
    scanf("%d", &i);
    if (i == 1) {
      fsm.sendEvent(KinovaFSM::Initialize);
      printf("Sent evInitialize\n");
    }
    else if(i == 2) {
      fsm.sendEvent(KinovaFSM::Shutdown);
      printf("Sent evShutdown\n");
    }
    fsm.process();
    usleep(1000000);
  }
  */

  /***************************/
  /* Testing CommandHandling */
  /***************************/
  /*
  communication.init();
  communication.process();
  usleep(200000);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(10);
  communication.debugSendEvent(KinovaFSM::SetMode,3);
  wait(10);
  communication.debugSendEvent(KinovaFSM::SetMode,4);
  wait(10);
  communication.debugSendEvent(KinovaFSM::SetMode,2);
  wait(10);
  */

  /********************/
  /* Testing Steering */
  /********************/
  /*
  communication.init();
  wait(1);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(5);
  communication.debugSendEvent(KinovaFSM::SetModeTranslation);
  wait(5);
  move(1000,0,0,5);
  move(-1000,0,0,5);
  move(0,1000,0,5);
  move(0,-1000,0,5);
  move(0,0,1000,5);
  move(0,0,-1000,5);
  */

  /*********************/
  /* Testing MoveToPos */
  /*********************/
  /*
  communication.init();
  wait(1);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(20);
  communication.debugSendEvent(KinovaFSM::SetMode,1);
  wait(5);
  communication.debugSendEvent(KinovaFSM::GoToPosition,1);
  wait(20);
  communication.debugSendEvent(KinovaFSM::GoToPosition,2);
  wait(20);
  */

  /***********************/
  /* Testing getPosition */
  /***********************/
  /*
  communication.init();
  wait(1);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(5);
  communication.debugSendEvent(KinovaFSM::SetModeTranslation);
  wait(5);
  communication.debugPrintPosition();
  wait(5);
  */

  /*******************************/
  /* Testing TeachState Movement */
  /*******************************/
  /*
  communication.init();
  wait(1);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(30);
  communication.debugSendEvent(KinovaFSM::GoToPosition,1);
  wait(20);
  communication.debugSendEvent(KinovaFSM::Teach,1);
  wait(5);
  move(0,500,0,5);
  move(0,-500,0,5);
  communication.debugSendEvent(KinovaFSM::SetMode,3);
  wait(5);
  move(0,500,0,5);
  move(0,-500,0,5);
  communication.debugSendEvent(KinovaFSM::Exit);
  wait(5);
  move(0,500,0,5);
  move(0,-500,0,5);
  */

  /**********************/
  /* Testing TeachState */
  /**********************/
  
  communication.init();
  wait(1);
  communication.debugSendEvent(KinovaFSM::Initialize);
  wait(30);
  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go to Bell
  wait(20);
  communication.debugSendEvent(KinovaFSM::GoToPosition,1); //Go to Home
  wait(20);
  communication.debugSendEvent(KinovaFSM::Teach,2);        //change to Teach Mode Bell:0
  wait(5);
  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go To Bell:0
  wait(20);
  move(200,-500,0,5);                                      //Move around
  wait(5);
  communication.debugSendEvent(KinovaFSM::SavePoint);      //Save current Position to Bell:0
  wait(5);
  move(-200,0,0,5);                                         //Move away.
  wait(5);
  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go to newly saved Bell:0
  wait(20);
  communication.debugSendEvent(KinovaFSM::Next);           //Advance to Teach Mode Bell:1
  wait(5);
  move(0,0,500,5);                                         //Move forward by 500.
  wait(5);
  communication.debugSendEvent(KinovaFSM::SavePoint);      //Save current Position to Bell:1
  wait(5);
  communication.debugSendEvent(KinovaFSM::Exit);
  wait(5);
  communication.debugSendEvent(KinovaFSM::GoToPosition,1); //Go to Home
  wait(20);
  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go to Bell
  wait(20);
  
}


