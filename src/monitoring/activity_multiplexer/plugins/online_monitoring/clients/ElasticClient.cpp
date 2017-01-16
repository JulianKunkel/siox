/*
 * =====================================================================================
 *
 *       Filename:  ElasticClient.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:33:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ElasticClient.hpp"

void ElasticClient::init(const std::string& host, const std::string& port, const std::string& username, const std::string& password) {
	DEBUGFUNC;
	Client::init(host, port, username, password);

	boost::asio::ip::tcp::resolver::query q{m_host, m_port};
	const std::string request = make_http_request("HEAD /" + m_index + "/_mapping/" + m_type, m_base64, m_host, m_port, "");

	m_resolv.async_resolve(q, boost::bind(&Client::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	m_ioservice.reset();
	m_ioservice.run();

	try {
		// Check if index exists
		unsigned int http_err_code = 0;
		size_t count = 0; // number of tries

		do {
			m_tcp_socket.send(boost::asio::buffer(request.data(), request.length()));
			const size_t bytes_transferred = boost::asio::read_until(m_tcp_socket, m_response, "\r\n");	
			m_response.commit(bytes_transferred);
			std::istream is(&m_response);
			std::string skip;
			is >> skip >> http_err_code >> skip;
//			std::cout << "http_err_code " << http_err_code << std::endl;
			m_tcp_socket.close();

			// Create index if not exists
			if (200 != http_err_code) {
				m_resolv.async_resolve(q, boost::bind(&Client::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
				m_ioservice.reset();
				m_ioservice.run();
				const std::string create_request = make_http_request("PUT /" + m_index, m_base64, m_host, m_port, m_mappings);
				m_tcp_socket.send(boost::asio::buffer(create_request.data(), create_request.length()));
				const size_t bytes_transferred = boost::asio::read_until(m_tcp_socket, m_response, "\r\n");	

				m_response.commit(bytes_transferred);
				std::istream is(&m_response);
				std::string result_line;
				while (std::getline(is, result_line)) {
//					std::cout << result_line << std::endl;
				}
				m_tcp_socket.close();
			}
		} while (200 != http_err_code || 5 > count++);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	sleep(1);
}


std::string ElasticClient::make_send_uri() {
	std::stringstream ss;
	ss << "POST /" << m_index << "/" << m_type;
	return ss.str();
}


std::string ElasticClient::to_json(std::shared_ptr<Client::Datapoint> point) const {
	using namespace std;	
	using namespace std::chrono;
	const string sep{", "};
	stringstream ss;

	const double perf = (point->m_duration) != 0 ? static_cast<double>(point->m_bytes) / static_cast<double>(point->m_duration) : 0;
	const double bytes_per_call = (point->m_calls) != 0 ? static_cast<double>(point->m_bytes) / static_cast<double>(point->m_calls) : 0;

	ss << "{" 
		<< to_json_snippet("duration", point->m_duration) << sep
		<< to_json_snippet("bytes", point->m_bytes) << sep
		<< to_json_snippet("calls", point->m_calls) << sep
		<< to_json_snippet("perf", perf) << sep
		<< to_json_snippet("bytes_per_call", bytes_per_call) << sep
		<< to_json_snippet("access", point->m_access) << sep
		<< to_json_snippet("filename", point->m_filename) << sep
		<< to_json_snippet("host", point->m_host) << sep
		<< to_json_snippet("jobid", point->m_jobid) << sep
		<< to_json_snippet("procid", point->m_procid) << sep
		<< to_json_snippet("username", point->m_username) << sep
		<< to_json_snippet("timestamp", duration_cast<milliseconds>(point->m_timestamp.time_since_epoch()).count()) <<
		"}";

	return ss.str();
}


std::string ElasticClient::get_json_mappings() const {
std::stringstream ss;
const std::string properties = R"|(
"duration" : {
	"type" : "long"
},
"bytes" : {
	"type" : "long"
},
"calls" : {
	"type" : "long"
},
"perf" : {
	"type" : "double"
},
"bytes_per_call" : {
	"type" : "double"
},
"access" : {
	"type" : "keyword"
},
"filename" : {
	"type" : "keyword"
},
"host" : {
	"type" : "keyword"
},
"jobid" : {
	"type" : "long"
},
"procid" : {
	"type" : "long"
},
"username" : {
	"type" : "keyword"
},
"timestamp" : {
	"type" : "date",
	"format":"epoch_millis"
})|";
ss << "{\"mappings\":{\"" << m_type << "\" : {\"properties\": {" << properties << "}}}}";
return ss.str();
}
