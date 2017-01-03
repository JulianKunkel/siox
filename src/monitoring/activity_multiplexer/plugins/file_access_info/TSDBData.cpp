/*
 * =====================================================================================
 *
 *       Filename:  TSDBData.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/23/2016 12:54:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "TSDBData.hpp"

#include <chrono>
#include <sstream>

Datapoint::Datapoint(const std::string& metric, long timestamp, double value, const std::vector<std::tuple<std::string, std::string>>& tags) :m_metric{metric}, m_timestamp{timestamp},  m_value{value}, m_tags{tags} {}



std::string 
Datapoint::to_json() const {
	std::stringstream ss;
	ss << "{" 
		<< "\"metric\":\"" << m_metric << "\","
		<< "\"timestamp\":\"" << m_timestamp << "\","
		<< "\"value\":\"" << m_value << "\","
		<< "\"tags\":{";

	size_t count = m_tags.size();
	for (const auto tag : m_tags) {
		ss << "\"" << std::get<0>(tag) << "\":\"" << std::get<1>(tag) << "\"";
		if (--count) {
			ss << ",";
		}
	}
	ss << "}}";
	return ss.str();
}



std::string 
DatapointSet::to_json() const {
	std::stringstream ss;
	ss << "[";
	size_t count = m_datapoints.size();
	for (const auto dp : m_datapoints) {
		ss << dp.to_json();
		if (--count) {
			ss << ",";
		}
	}
	ss << "]";
	return ss.str();
}



void 
DatapointSet::append(const Datapoint& dp) {
	m_datapoints.push_back(dp);
}
