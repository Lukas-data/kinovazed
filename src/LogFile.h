#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <stdio.h>
#include <string>
#include <fstream>
#include "Log.h"

#define REP_LEVEL_FILE    logINFO
#define REP_LEVEL_CONSOLE logINFO

#define LOG_FILEPATH "/home/kinova/Documents/kinovazed/logfiles/"
#define LOGARCHIVE_FILEPATH "/home/kinova/Documents/kinovazed/logfiles/archive/"
#define LOG_NAME "log"
#define LOG_SUFFIX ".txt"

class LogFile {
  
  public :
    static bool create();

};
inline bool LogFile::create() {
  CONSLog::ReportingLevel() = REP_LEVEL_CONSOLE;

  std::string filepath = std::string(LOG_FILEPATH) + LOG_NAME + LOG_SUFFIX;
  std::ifstream file(filepath.c_str());
  if (file) {
    int i = 0;
    for (int i = 0; i < 10000; i++) {
      char number[5];
      snprintf(number, 5, "%04d", i);
      std::string archfilepath = std::string(LOGARCHIVE_FILEPATH) + LOG_NAME + number + LOG_SUFFIX;
      std::ifstream archfile(archfilepath.c_str());
      if (!archfile) { 
        rename(filepath.c_str(), archfilepath.c_str());
        break;
      }
    }
  }
  std::ifstream newfile(filepath.c_str());
  if (!newfile) {
    Output2FILE::Stream() = fopen(filepath.c_str(), "a");
    FILELog::ReportingLevel() = REP_LEVEL_FILE;
    return true;
  }
  else {
    printf("ERROR at creating Log-File!\n");
    return false;
  }
}


#endif
