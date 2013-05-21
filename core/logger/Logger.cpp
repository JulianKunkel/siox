#include "Logger.h"

Logger::Logger() {}

void Logger::log(const Priority prio, const std::string &msg)
{
	switch(prio) {
	case Priority::EMERG:
	case Priority::ALERT:
	case Priority::CRIT:
	case Priority::ERR:
		std::cerr << msg << std::endl;
	default:
		std::cout << msg << std::endl;
	}
}