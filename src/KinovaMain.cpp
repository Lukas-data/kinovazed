
/***************************************************************************
 *  Kinova Main
 *
 *  Created: Fri Jun 15 2018
 ****************************************************************************/


#include <stdio.h>
#include "LogFile.h"
#include "Log.h"
#include "CommandHandling.h"


CommandHandling commandHandler;


int main(int argc, char *argv[])
{
  if ( LogFile::create() ) {
    ALL_LOG(logINFO) << "KinovaMain - Startup!";

    commandHandler.init();     
    while (1) {
      try {
        commandHandler.process();
      }
      catch ( const std::runtime_error& e) {
        ALL_LOG(logERROR) << "RuntimeError: " << e.what();
        return -1;
      }
      usleep(10000);
    }
  }
  usleep(10000000);
  return -1;
}
