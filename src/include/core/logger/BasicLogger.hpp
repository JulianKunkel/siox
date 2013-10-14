#ifndef BASIC_LOGGER_H
#define BASIC_LOGGER_H

#define MAX_LOG_LENGTH 256

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string>

class LoggerException
		: public std::exception {

	public:
		LoggerException( const char * err_msg ) : err_msg_( err_msg ) {}
		const char * what() const throw() {
			return err_msg_;
		}
	private:
		const char * err_msg_;

};


class BasicLogger {

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

		BasicLogger();
		~BasicLogger();

		void log( const Priority prio, const char * format, ... );

	protected:
		void log_append( const Priority prio, const char * buffer );

};

#endif