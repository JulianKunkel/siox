
#include <chrono>

#include "TSDBClient.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;



std::string encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}



std::string make_http_request(const std::string& enc, const std::string& host, const std::string& port, const std::string& json) {
	std::stringstream ss;
	ss << "POST /api/put\?details HTTP/1.1\r\n";
	ss << "Host: " << host << ":" << port << "\r\n";
	ss << "Content-Type: application/json\r\n";
	ss << "Authorization: Basic " << enc << "\r\n";
//			ss << "Accept: text/plain\r\n";
//			ss << "Accept-Encoding: gzip, deflate\r\n";
	ss << "Accept: */* \r\n";
	ss << "Content-Length: " << json.length() << "\r\n";
	ss << "Connection: close\r\n\r\n";
	ss << json;

	return ss.str();
}



std::string make_http_request(const std::string& enc, const std::string& host, const std::string& port, const DatapointSet& dps) {
	return make_http_request(enc, host, port, dps.to_json());
}


TSDBClient::TSDBClient(): m_ioservice{}, m_tcp_socket{m_ioservice}, m_resolv{m_ioservice} {}

void TSDBClient::init(const std::string& host, const std::string& port, const std::string& username, const std::string& password) {
		m_host = host;
		m_port = port;
		std::stringstream ss;
		ss << username << ":" << password;
		m_base64 = encode64(ss.str());
	}



void 
TSDBClient::response_read_handler (const boost::system::error_code& ec, size_t bytes_transferred) {
	DEBUGFUNC;
	m_response.commit(bytes_transferred);
	std::istream is(&m_response);
	std::string result_line;
  while (std::getline(is, result_line)) {
    std::cout << result_line << std::endl;
  }
}



void 
TSDBClient::response_handler (const boost::system::error_code &ec) {
	DEBUGFUNC;
	if (!ec) {
//		boost::asio::async_read_until(m_tcp_socket, m_response, "\r\n", 
		boost::asio::async_read_until(m_tcp_socket, m_response, ' ', 
				boost::bind(&TSDBClient::response_read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));	
	}
	else {
		std::cout << "leave response handler\n";
	}
}



void 
TSDBClient::connect_handler (const boost::system::error_code &ec) {
	DEBUGFUNC;
	if (!ec) {
		if (!m_pending.empty()) {

			DatapointSet dps;
			for (const auto dp : m_pending) {
				dps.append(m_pending.front());
				m_pending.pop_front();
			}

			const std::string request = make_http_request(m_base64, m_host, m_port, dps);
			std::cout << request << std::endl;
			
			m_tcp_socket.async_send(
					buffer(request.data(), request.length()), boost::bind(&TSDBClient::connect_handler, this,  boost::asio::placeholders::error));

			boost::asio::async_read_until(m_tcp_socket, m_response, "\r\n", 
					boost::bind(&TSDBClient::response_read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));	
		}
		else {
//			std::cout << "leave connect handler\n";
		}
	}
}



void 
TSDBClient::resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator ep_it) {
	if (!ec) {
		m_tcp_socket.async_connect(*ep_it, boost::bind(&TSDBClient::connect_handler, this, boost::asio::placeholders::error));
	}
	else {
		std::cout << "couldn't m_resolve\n";
	}
}



void 
TSDBClient::enque(const Datapoint& dp) {
	m_pending.push_back(dp);		
}



void
TSDBClient::send() {
	tcp::resolver::query q{m_host, m_port};
	m_resolv.async_resolve(q, boost::bind(&TSDBClient::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	m_ioservice.reset();
	auto start = std::chrono::high_resolution_clock::now();
	m_ioservice.run();
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long delta = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << delta << " micro seconds" << std::endl;
	m_tcp_socket.close();
}
