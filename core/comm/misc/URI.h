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
private:
	std::string protocol, host, port;
	void parse(const std::string &url_s);
public:
	URI(const std::string &url_s);
	std::string get_protocol() const;
	std::string get_host() const;
	std::string get_path() const;
	std::string get_port() const;
};

#endif