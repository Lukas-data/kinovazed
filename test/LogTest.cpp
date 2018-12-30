
/***************************************************************************
 *  LogTest
 *
 *  Created: Dez 29 2018
 ****************************************************************************/

#include <stdio.h>
#include "LogFile.h"
#include "Log.h"
#include "LogTestSubClass.h"

int main(int argc, char *argv[])
{
  LogTestSubClass sc;
  LogFile::create();

  const int count = 3;
  sc.error();
  FILE_LOG(logWARNING) <<"LogWARNING: A loop with " << count << " iterations";
  for (int i = 0; i < count; i++) {
    FILE_LOG(logINFO) << "LogINFO: the counter i = " << i;
  }
  sc.debug(0);
  sc.debug(1);
  sc.debug(2);
  sc.debug(3);

/*
  CONS_LOG(logERROR) << "LogERROR: Oooppss";
  CONS_LOG(logWARNING) <<"LogWARNING: A loop with " << count << " iterations";
  for (int i = 0; i < count; i++) {
    CONS_LOG(logINFO) << "LogINFO: the counter i = " << i;
  }
  CONS_LOG(logDEBUG) << "Debug";
  CONS_LOG(logDEBUG1) << "Debug1";
  CONS_LOG(logDEBUG2) << "Debug2";
  CONS_LOG(logDEBUG3) << "Debug3";*/
}


