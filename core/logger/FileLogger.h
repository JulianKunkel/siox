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
	
	void log(const Priority prio, const std::string &msg);

private:
	std::ofstream logfile_;
};

#endif

