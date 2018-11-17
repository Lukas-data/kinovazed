
/***************************************************************************
 *  Kinova Main
 *
 *  Created: Fri Jun 15 2018
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>
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


}


