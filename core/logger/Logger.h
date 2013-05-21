#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

class LoggerException 
   : public std::exception {
	  
public:
	LoggerException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class Logger {

public:

	enum Priority {
		EMERG,
		ALERT,
		CRIT,
		ERR,
		WARNING,
		NOTICE,
		INFO,
		DEBUG
	};
	
	Logger();
	~Logger();
	
	void log(const Priority prio, const std::string &msg);
	
};

#endif