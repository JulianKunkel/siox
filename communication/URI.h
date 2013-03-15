/**
 * @file URI.h Uniform Resource Identifiers
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef URI_H
#define URI_H

#include <string>

/**
 * Helper class to parse connection addresses like <i>protocol://host:port</i>
 */
class URI {
public:
	std::string protocol, host, port;
	URI(const std::string &url_s);
private:
	void parse(const std::string &url_s);
	
};

#endif