
/***************************************************************************
 *  Kinova Main
 *
 *  Created: Fri Jun 15 2018
 ****************************************************************************/


#include <stdio.h>
#include "CommandHandling.h"


CommandHandling commandHandler;


int main(int argc, char *argv[])
{

  printf("\n");
  printf("/-----------------\\\n");
  printf("| KinovaRaspiMain | \n");
  printf("\\-----------------/\n\n");

  commandHandler.init();     
  while (1) {
    try {
      commandHandler.process();
    }
    catch ( const std::runtime_error& e) {
      printf("RuntimeError: %s\n", e.what());
      printf("restarting...\n");
      return -1;
    }
    usleep(10000);
  }
}
