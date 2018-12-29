/*Source: Dr.Dobb's Loggin in C++: www.drdobbs.com/cpp/logging-in.c/201804215
(consultation: 29. Dez 2018)*/

#ifndef _LOG_H_
#define _LOG_H_


#include <sstream>
#include <string>
#include <stdio.h>


enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4};

#define LOG(level) \
if (level > Log::ReportingLevel() ) ; \
else Log().Get(level)

class Log {

  public:
    Log();
    virtual ~Log();
    std::ostringstream& Get(TLogLevel level = logINFO);
    
    static TLogLevel& ReportingLevel();
    
  protected:
    std::ostringstream oss;

  private:
    Log(const Log&);
    Log& operator =(const Log&);  
};
Log::Log() {}

std::ostringstream& Log::Get(TLogLevel level) {
  oss << std::string((level < logDEBUG ? level : logDEBUG)*2, ' ');
  //oss << ToString(level) << ": ";
  return oss;  
}

Log::~Log() {
  oss << std::endl;
  fprintf(stderr, "%s", oss.str().c_str());
  fflush(stderr);
}

TLogLevel& Log::ReportingLevel() {
  static TLogLevel reportingLevel(logINFO);
  return reportingLevel;
}


#endif
