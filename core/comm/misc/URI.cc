#include "URI.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <string>

// Public 

URI::URI(const std::string &url_s)
{
	parse(url_s);
}


std::string URI::get_protocol() const
{
	return protocol;
}


std::string URI::get_host() const 
{
	return host;
}


std::string URI::get_path() const 
{
	return host;
}


std::string URI::get_port() const
{
	return port;
}

// Private

void URI::parse(const std::string &url_s)
{
	const std::string proto_end("://");
	std::string::const_iterator proto_i = search(url_s.begin(), url_s.end(), 
						     proto_end.begin(), 
						     proto_end.end());
	protocol.reserve(distance(url_s.begin(), proto_i));
	transform(url_s.begin(), proto_i, back_inserter(protocol),
		  std::ptr_fun<int, int>(tolower));
	
	if (proto_i == url_s.end())
		return;
	
	advance(proto_i, proto_end.length());
	std::string::const_iterator port_i = find(proto_i, url_s.end(), ':');
	host.reserve(distance(proto_i, port_i));
	transform(proto_i, port_i, back_inserter(host), 
		  std::ptr_fun<int, int>(tolower));
	
	if (port_i != url_s.end())
		++port_i;
	
	port.assign(port_i, url_s.end());
}
