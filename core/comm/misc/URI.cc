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
	
	if (malformed())
		throw(new MalformedURIException("Malformed URI."));
	
}


std::string URI::protocol() const
{
	return protocol_;
}


std::string URI::host() const 
{
	return host_;
}


std::string URI::path() const 
{
	return host_;
}


std::string URI::port() const
{
	return port_;
}

// Private

void URI::parse(const std::string &url_s)
{
	const std::string proto_end("://");
	std::string::const_iterator proto_i = search(url_s.begin(), url_s.end(), 
						     proto_end.begin(), 
						     proto_end.end());
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


int URI::malformed()
{
	if (protocol() != "ipc" && protocol() != "tcp")
		return 1;
	
	if (protocol() == "ipc" && path().empty())
		return 1;
		
	if (protocol() == "tcp" && (host().empty() || port().empty())) 
		return 1;
	
	return 0;
}
