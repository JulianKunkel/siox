#ifndef SYSLOG_LOGGER_H
#define SYSLOG_LOGGER_H

#include <string>
#include <syslog.h>

#include <core/logger/Logger.hpp>

class SyslogLogger
		: public Logger {

	public:
		SyslogLogger( const std::string & logname );

		~SyslogLogger();

	protected:
		void log_append( const Priority prio, const char * buffer );

};


#endif
