/**
 * @file URI.hpp Uniform Resource Identifiers
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef URI_H
#define URI_H

#include <algorithm>
#include <cctype>
#include <exception>
#include <functional>
#include <iostream>
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
	
	inline void parse(const std::string &url_s)
	{
		const std::string proto_end("://");
		std::string::const_iterator proto_i = search(url_s.begin(), 
			url_s.end(), proto_end.begin(), proto_end.end());
		protocol_.reserve(distance(url_s.begin(), proto_i));
		transform(url_s.begin(), proto_i, back_inserter(protocol_),
			std::ptr_fun<int, int>(tolower));
		
		if (proto_i == url_s.end())
			return;
		
		advance(proto_i, proto_end.length());
		std::string::const_iterator port_i = find(proto_i, url_s.end(), ':');
		host_.reserve(distance(proto_i, port_i));
		transform(proto_i, port_i, back_inserter(host_), 
			std::ptr_fun<int, int>(tolower));
		
		if (port_i != url_s.end())
			++port_i;
		
		port_.assign(port_i, url_s.end());
	}

	
	inline int malformed()
	{
		if (protocol() != "ipc" && protocol() != "tcp")
			return 1;
		
		if (protocol() == "ipc" && path().empty())
			return 1;
			
		if (protocol() == "tcp" && (host().empty() || port().empty())) 
			return 1;
		
		return 0;
	}

public:
	/**
	 * Creates an URI object containing the given URI. Supported URI are
	 * ipc://path-to-ipc-socket and tcp://ip-address:port-number.
	 * 
	 * @throws MalformedURIException
	 */
	URI(const std::string &url_s) 
	{
		parse(url_s);
		
		if (malformed())
			throw(new MalformedURIException("Malformed URI."));
		
	}
	
	inline std::string protocol() const 
	{
		return protocol_;
	}
	
	
	inline std::string host() const
	{
		return host_;
	}

	
	inline std::string path() const
	{
		return host_;
	}

	
	inline std::string port() const
	{
		return port_;
	}

};

#endif