#include "LogTestSubClass.h"

#include "Log.h"

void LogTestSubClass::error() {
  ALL_LOG(logERROR) << "AllLogERROR: SubClass";
  //FILE_LOG(logERROR) << "FileLogERROR: SubClass";
}

void LogTestSubClass::debug(int n) {
  ALL_LOG(static_cast<TLogLevel>(logDEBUG + n) ) << "AllLogDebug" << n << ": SubClass";
  //FILE_LOG(static_cast<TLogLevel>(logDEBUG + n) ) << "FileLogDebug: SubClass";
}

