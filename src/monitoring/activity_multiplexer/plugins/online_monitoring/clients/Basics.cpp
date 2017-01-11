/*
 * =====================================================================================
 *
 *       Filename:  Basics.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:38:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "../Debug.hpp"
#include "Basics.hpp"

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <sstream>

std::string encode64(const std::string &val) {
	using namespace boost::archive::iterators;
	using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
	auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
	return tmp.append((3 - val.size() % 3) % 3, '=');
}


std::string make_http_request(const std::string& verb_uri, const std::string& enc, const std::string& host, const std::string& port, const std::string& payload) {
	std::stringstream ss;
	ss << verb_uri + " HTTP/1.1\r\n";
	ss << "Host: " << host << ":" << port << "\r\n";
	ss << "Content-Type: application/json\r\n";
	ss << "Authorization: Basic " << enc << "\r\n";
//			ss << "Accept: text/plain\r\n";
//			ss << "Accept-Encoding: gzip, deflate\r\n";
	ss << "Accept: */* \r\n";
	ss << "Content-Length: " << payload.length() << "\r\n";
	ss << "Connection: close\r\n\r\n";
	ss << payload;

	return ss.str();
}

std::string embrace(const std::string& snippet) {
	std::stringstream ss;
	ss << "{" << snippet << "}";
	return ss.str();
}
