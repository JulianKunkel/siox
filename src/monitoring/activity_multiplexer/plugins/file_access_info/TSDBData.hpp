/*
 * =====================================================================================
 *
 *       Filename:  TSDBData.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/23/2016 12:54:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  TSDBData_INC
#define  TSDBData_INC

#include <vector>
#include <tuple>
#include <string>

class Datapoint {
	public:
		Datapoint(const std::string& metric, long timestamp, double value, const std::vector<std::tuple<std::string, std::string>>& tags);
		std::string to_json() const;

	private:
		std::string m_metric;
		long m_timestamp;
		double m_value;
		std::vector<std::tuple<std::string, std::string>> m_tags;
};


class DatapointSet {
	public:
		std::string to_json() const;
		void append(const Datapoint& dp);

	private:
		std::vector<Datapoint> m_datapoints;
};

#endif   /* ----- #ifndef TSDBData_INC  ----- */
