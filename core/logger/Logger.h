#ifndef LOGGER_H
#define LOGGER_H

#include <boost/ptr_container/ptr_list.hpp>

#include "BasicLogger.h"

class Logger
   : public BasicLogger {
	   
public:
	Logger();
	~Logger();
	
	void add_logger(Logger *logger);
	void clear_loggers();

protected:
	void log_append(const Priority prio, const char *buffer);
	
private:
	boost::ptr_list<Logger> loggers_;   
	
};

#endif