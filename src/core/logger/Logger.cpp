#include <core/logger/Logger.hpp>


Logger::Logger()
{
}


Logger::~Logger()
{
}


void Logger::add_logger( Logger * logger )
{
	loggers_.push_back( logger );
}


void Logger::clear_loggers()
{
	loggers_.clear();
}


void Logger::log_append( const Priority prio, const char * buffer )
{
	boost::ptr_list<Logger>::iterator i;
	for( i = loggers_.begin(); i != loggers_.end(); ++i ) {
		( *i ).log( prio, buffer );
	}
}
