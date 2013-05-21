#ifndef SYSLOG_LOGGER_H
#define SYSLOG_LOGGER_H

#include <string>
#include <syslog.h>

#include "Logger.h"

class SyslogLogger 
   : public Logger {
	
public:
	SyslogLogger(const std::string &logname);
	
	~SyslogLogger();
	
	void log(const Priority, const std::string &msg);
	
};

#endif
