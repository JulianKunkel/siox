#include <core/logger/SioxLogger.hpp>

void SioxLogger::add_logger(Logger &logger)
{
	loggers_.push_back(&logger);
}


void SioxLogger::clear_loggers()
{
	loggers_.clear();
}


void SioxLogger::log(const Priority prio, const std::string &msg)
{
	boost::ptr_list<Logger>::iterator i;
	for (i = loggers_.begin(); i != loggers_.end(); ++i) {
		(*i).log(prio, msg);
	}
}
