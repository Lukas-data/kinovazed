
/***************************************************************************
 *  LogTest
 *
 *  Created: Dez 29 2018
 ****************************************************************************/

#include <stdio.h>
#include "Log.h"

int main(int argc, char *argv[])
{
  Log::ReportingLevel() = logDEBUG2;
  const int count = 3;
  LOG(logERROR) << "LogERROR: Oooppss";
  LOG(logWARNING) <<"LogWARNING: A loop with " << count << " iterations";
  for (int i = 0; i < count; i++) {
    LOG(logINFO) << "LogINFO: the counter i = " << i;
  }
  LOG(logDEBUG) << "Debug";
  LOG(logDEBUG1) << "Debug1";
  LOG(logDEBUG2) << "Debug2";
  LOG(logDEBUG3) << "Debug3";
}


