#ifndef __SIOX_STDEXCEPTION_HPP
#define __SIOX_STDEXCEPTION_HPP

#include <exception>
#include <string>

class NotFoundError: public std::exception {
	public:
		std::string msg;

		NotFoundError() {
			msg = "Error Attribute/Value NotFound";
		}

		NotFoundError( const std::string & msg ) {
			this->msg = msg;
		}

		virtual const char * what() const throw() {
			return msg.c_str();
		}
};


class IllegalStateError: public std::exception {
	public:
		std::string msg;

		IllegalStateError( const std::string & msg ) {
			this->msg = msg;
		}

		virtual const char * what() const throw() {
			return msg.c_str();
		}
};


class IllegalArgumentError: public std::exception {
	public:
		std::string msg;

		IllegalArgumentError( const std::string & msg ) {
			this->msg = msg;
		}

		virtual const char * what() const throw() {
			return msg.c_str();
		}
};



#endif