#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "Log.h"

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
		CONS_LOG(logINFO) << "Log-file: '" << filepath << "' already exists.";
		for (int i = 0; i < 10000; i++) {
			auto num { std::to_string(i) };
			std::string archfilepath = std::string(LOGARCHIVE_FILEPATH) //
					+ LOG_NAME //
					+ std::string(4 - num.length(), '0') //
					+ num //
					+ LOG_SUFFIX;
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
			CONS_LOG(logINFO) << "Creating new log-file: '" << filepath << "'";
			Output2FILE::Stream() = logFile;
			FILELog::ReportingLevel() = REP_LEVEL_FILE;
			return true;
		}
	}
	CONS_LOG(logERROR) << "ERROR at creating log-file!";
	return false;
}

#endif
