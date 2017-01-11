
#include <chrono>

#include "../Debug.hpp"
#include "Basics.hpp"
#include "TSDBClient.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;

std::string TSDBClient::make_send_uri() {
	DEBUGFUNC;
	return "POST /api/put\?details";
}

std::string TSDBClient::to_json_metric(const std::string& metric, LongType value, const std::shared_ptr<Client::Datapoint> point) const {
	using namespace std::chrono;
	const std::string sep{", "};
	std::stringstream ss;
	ss << "{" 
		<< to_json_snippet("metric", std::string{"siox.filestats.io."} + metric) << sep
		<< to_json_snippet("timestamp", duration_cast<seconds>(point->m_timestamp.time_since_epoch()).count()) << sep
		<< to_json_snippet("value", value) << sep
		<< "\"tags\":{"
		<< to_json_snippet("access", point->m_access) << sep
		<< to_json_snippet("host", point->m_host) << sep
		<< to_json_snippet("jobid", point->m_jobid) << sep
		<< to_json_snippet("procid", point->m_procid) << sep
		<< to_json_snippet("username", point->m_username) << sep
		<< to_json_snippet("filename", point->m_filename)
		<< "}}";
	return ss.str();
}


std::string TSDBClient::to_json(const std::shared_ptr<Client::Datapoint> point) const {
	const std::string sep{", "};
	std::stringstream ss;
	ss << "[" 
		<< to_json_metric("bytes", point->m_bytes, point) << sep
		<< to_json_metric("calls", point->m_calls, point) << sep
		<< to_json_metric("duration", point->m_duration, point)
		<< "]";
	return ss.str();
}
