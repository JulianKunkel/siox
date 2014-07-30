#include <core/logger/SyslogLogger.hpp>

SyslogLogger::SyslogLogger( const std::string & logname )
{
	setlogmask( LOG_UPTO( LOG_DEBUG ) );
	openlog( logname.c_str(), LOG_CONS | LOG_NDELAY | LOG_NOWAIT
	         | LOG_PERROR | LOG_PID, LOG_USER );
}


SyslogLogger::~SyslogLogger()
{
	closelog();
}


void SyslogLogger::log_append( const Priority prio, const char * buffer )
{
	syslog( prio, "%s", buffer );
}


