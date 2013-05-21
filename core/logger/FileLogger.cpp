#include "FileLogger.h"

FileLogger::FileLogger(const std::string &path)
{
	logfile_.open(path.c_str(), std::ios::out | std::ios::app);
	
	if (!logfile_.is_open())
		throw(new LoggerException("Error opening log file.")); 
	
}


FileLogger::~FileLogger()
{
	logfile_.close();
}


void FileLogger::log(const Priority prio, const std::string &msg)
{
	logfile_ << prio << ": " << msg;
}

