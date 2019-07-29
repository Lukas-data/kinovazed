#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "Log.h"

#include <stdio.h>
#include <string>
#include <fstream>

#define REP_LEVEL_FILE    logDEBUG4
#define REP_LEVEL_CONSOLE logDEBUG2

#define LOG_FILEPATH "logfiles/"
#define LOGARCHIVE_FILEPATH "logfiles/archive/"
#define LOG_NAME "log"
#define LOG_SUFFIX ".txt"

class LogFile {

public:
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
		FILE *logFile = fopen(filepath.c_str(), "a");
		if (logFile) {
			Output2FILE::Stream() = logFile;
			FILELog::ReportingLevel() = REP_LEVEL_FILE;
			return true;
		}
	}
	printf("ERROR at creating Log-File!\n");
	return false;
}

#endif
