#include "SyslogLogger.h"

SyslogLogger::SyslogLogger(const std::string &logname) 
{
	setlogmask(LOG_UPTO(LOG_DEBUG));
	openlog(logname.c_str(), LOG_CONS | LOG_NDELAY | LOG_NOWAIT | LOG_PERROR | 
		LOG_PID, LOG_USER);
}


SyslogLogger::~SyslogLogger()
{
	closelog();
}


void SyslogLogger::log(const Priority prio, const std::string &msg)
{
	syslog(prio, msg.c_str());
}


