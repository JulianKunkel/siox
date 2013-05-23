#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

#include "Logger.h"

class FileLogger 
   : public Logger {

public:
	FileLogger(const std::string &path);
	~FileLogger();

protected:
	void log_append(const Priority prio, const char *buffer);

private:
	std::ofstream logfile_;
};

#endif

