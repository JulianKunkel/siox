/*
 * =====================================================================================
 *
 *       Filename:  Basics.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:39:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  Basics_INC
#define  Basics_INC

#include "../Debug.hpp"
#include <sstream>
#include <string>

std::string encode64(const std::string &val);
std::string make_http_request(const std::string& verb_uri, const std::string& enc, const std::string& host, const std::string& port, const std::string& payload);

template <typename T>
std::string to_json_snippet(const std::string& key, const T& value) {
	std::stringstream ss;
	ss << "\"" << key << "\":\"" << value << "\"";
	return ss.str();
}

std::string embrace(const std::string& snippet);

#endif   /* ----- #ifndef Basics_INC  ----- */
