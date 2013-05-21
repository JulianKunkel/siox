#ifndef SIOX_LOGGER_H
#define SIOX_LOGGER_H

#include <boost/ptr_container/ptr_list.hpp>

#include "Logger.h"

class SioxLogger
   : public Logger {
	   
public:
	void add_logger(Logger &logger);
	void clear_loggers();
	
	void log(const Priority prio, const std::string &msg);
	
private:
	boost::ptr_list<Logger> loggers_;   
	
};

#endif