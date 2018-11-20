
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
  for (int i =0; i<n; i++) {
    communication.setJoystick(x,y,z);
    communication.process();
    usleep(200000);
  }
  communication.setJoystick(0,0,0);
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
  communication.debugSendEvent(KinovaFSM::SetModeAxis1);
  wait(10);
  communication.debugSendEvent(KinovaFSM::SetModeAxis2);
  wait(10);
  communication.debugSendEvent(KinovaFSM::SetModeRotation);
  wait(10);
  */

  /********************/
  /* Testing Steering */
  /********************/
  struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
  char button;
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
}


