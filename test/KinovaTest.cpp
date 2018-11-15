
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

int main(int argc, char *argv[])
{
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
  
  CommandHandling communication;
  communication.init();
  communication.sendEvent(KinovaFSM::Initialize);
  while (true) {
    communication.process();
    usleep(1000000);
  }
  
    
  

}
