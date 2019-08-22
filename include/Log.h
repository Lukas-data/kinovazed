/*Source: Dr.Dobb's Loggin in C++: www.drdobbs.com/cpp/logging-in.c/201804215
(consultation: 29. Dez 2018)*/

#ifndef _LOG_H_
#define _LOG_H_


#include <sstream>
#include <string>
#include <cstdio>


enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4};

static const char *TLogLevelName[] = {"ERROR", "WARNING", "INFO", "DEBUG0", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4" };

template <typename OutputPolicy>
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
    TLogLevel messageLevel; 

};
template <typename OutputPolicy>
Log<OutputPolicy>::Log() {}

template <typename OutputPolicy>
Log<OutputPolicy>::~Log() {
  OutputPolicy::Output(oss.str(), messageLevel);
}

template <typename OutputPolicy>
std::ostringstream& Log<OutputPolicy>::Get(TLogLevel level) {
  oss << std::string((level < logDEBUG ? level : logDEBUG)*2, ' ');
  oss << TLogLevelName[level] << ": ";
  messageLevel = level;
  return oss;  
}

template <typename OutputPolicy>
TLogLevel& Log<OutputPolicy>::ReportingLevel() {
  static TLogLevel reportingLevel(logINFO);
  return reportingLevel;
}

/******OutputPolicies******/

/*Output to FILE:
define File:
FILE* pFile = fopen("logfile.log", "a");
Output2FILE::Stream() = pFile;

Call: FILELog,
Write to Log: FILE_LOG(TLogLevel)*/
class Output2FILE {
  public:
    static FILE*& Stream();
    static void Output(const std::string& msg, TLogLevel messageLevel);
};
inline FILE*& Output2FILE::Stream() {
  static FILE* pStream = stderr;
  return pStream;
}
inline void Output2FILE::Output(const std::string& msg, TLogLevel messageLevel) {
  
  FILE* pStream = Stream();
  if (!pStream)
    return;

  fprintf(pStream, "%s\n", msg.c_str() );
  fflush(pStream);
}
typedef Log<Output2FILE> FILELog;
#define FILE_LOG(level) \
if (level > FILELog::ReportingLevel() || ! Output2FILE::Stream() ) ; \
else FILELog().Get(level)

/*Output to console:
Call: CONSLog,
Write to Log: CONS_LOG(TLogLevel)*/
class Output2CONSOLE {
  public:
    static void Output(const std::string& msg, TLogLevel messageLevel);
};
inline void Output2CONSOLE::Output(const std::string& msg, TLogLevel messageLevel) {
  FILE* pStream = stderr;

  fprintf(pStream, "%s\n", msg.c_str() );
  fflush(pStream);
}
typedef Log<Output2CONSOLE> CONSLog;
#define CONS_LOG(level) \
if (level > CONSLog::ReportingLevel() ) ; \
else CONSLog().Get(level)

/*Output to FILE & Console:
Write to Log: ALL_LOG(TLogLevel)*/
class Output2ALL {
  public:
    static void Output(const std::string& msg, TLogLevel messageLevel);
};
inline void Output2ALL::Output(const std::string& msg, TLogLevel messageLevel) {
  if ( messageLevel <= FILELog::ReportingLevel() ) {
    FILE* pStream = Output2FILE::Stream();
    if (!pStream)
      return;

    fprintf(pStream, "%s\n", msg.c_str() );
    fflush(pStream);
  }
  if ( messageLevel <= CONSLog::ReportingLevel() ) {
    FILE* pStream = stderr;

    fprintf(pStream, "%s\n", msg.c_str() );
    fflush(pStream);
  }
}
typedef Log<Output2ALL> ALLLog;
#define ALL_LOG(level) \
if ( (level > CONSLog::ReportingLevel() && level > FILELog::ReportingLevel() )|| ! Output2FILE::Stream() ) ; \
else ALLLog().Get(level)
#endif
