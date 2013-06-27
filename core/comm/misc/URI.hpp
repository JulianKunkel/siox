/**
 * @file URI.h Uniform Resource Identifiers
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef URI_H
#define URI_H

#include <exception>
#include <string>


class MalformedURIException : public std::exception {
public:
	MalformedURIException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


/**
 * Helper class to parse connection addresses like <i>protocol://host:port</i>
 */
class URI {
private:
	std::string protocol_, host_, port_;
	void parse(const std::string &url_s);
	int malformed();
public:
	/**
	 * Creates an URI object containing the given URI. Supported URI are
	 * ipc://path-to-ipc-socket and tcp://ip-address:port-number.
	 * 
	 * @throws MalformedURIException
	 */
	URI(const std::string &url_s);
	std::string protocol() const;
	std::string host() const;
	std::string path() const;
	std::string port() const;
};

#endif