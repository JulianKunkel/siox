#include <core/logger/BasicLogger.hpp>

BasicLogger::BasicLogger() {}
BasicLogger::~BasicLogger() {}

void BasicLogger::log(const Priority prio, const char *format, ...)
{
	va_list args;
	char buffer[MAX_LOG_LENGTH];
	
	va_start(args, format);
	vsprintf(buffer, format, args);
	
	log_append(prio, buffer);
	
	va_end(args);
}


void BasicLogger::log_append(const Priority prio, const char *buffer)
{
	switch(prio) {
	case Priority::EMERG:
	case Priority::ALERT:
	case Priority::CRIT:
	case Priority::ERR:
		std::cerr << buffer << std::endl;
	default:
		std::cout << buffer << std::endl;
	}
}

