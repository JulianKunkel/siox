/*
 * =====================================================================================
 *
 *       Filename:  Client.cpp
 * Description:  
 *    jk
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:47:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BaseClient.hpp"


Client::Client(): m_ioservice{}, m_tcp_socket{m_ioservice}, m_resolv{m_ioservice} {
	DEBUGFUNC;
}



void Client::init(const std::string& host, const std::string& port, const std::string& username, const std::string& password) {
	DEBUGFUNC;
	m_host = host;
	m_port = port;
	std::stringstream ss;
	ss << username << ":" << password;
	m_base64 = encode64(ss.str());
}



void 
Client::response_read_handler (const boost::system::error_code& ec, size_t bytes_transferred) {
	DEBUGFUNC;
	m_response.commit(bytes_transferred);
	std::istream is(&m_response);
	std::string result_line;
  while (std::getline(is, result_line)) {
	 std::cout << result_line << std::endl;
  }
}



void 
Client::response_handler (const boost::system::error_code &ec) {
	DEBUGFUNC;
	if (!ec) {
//		boost::asio::async_read_until(m_tcp_socket, m_response, "\r\n", 
		boost::asio::async_read_until(m_tcp_socket, m_response, ' ', 
				boost::bind(&Client::response_read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));	
	}
	else {
		std::cout << "leave response handler\n";
	}
}



void 
Client::connect_handler (const boost::system::error_code &ec) {
	DEBUGFUNC;
	if (!ec) {
		if (!m_pending.empty()) {
			while (true != m_pending.empty()) {
				const std::string send_uri = make_send_uri();
				const std::string request = make_http_request(send_uri, m_base64, m_host, m_port, to_json(m_pending.front()));
				m_pending.pop_front();
				std::cout << request << std::endl;
				m_tcp_socket.async_send(boost::asio::buffer(request.data(), request.length()), boost::bind(&Client::connect_handler, this,  boost::asio::placeholders::error));
				boost::asio::async_read_until(m_tcp_socket, m_response, "\r\n", boost::bind(&Client::response_read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));	
			}
		}
		else {
//			std::cout << "leave connect handler\n";
		}
	}
}



void 
Client::resolve_handler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator ep_it) {
	DEBUGFUNC;
	if (!ec) {
		m_tcp_socket.async_connect(*ep_it, boost::bind(&Client::connect_handler, this, boost::asio::placeholders::error));
	}
	else {
		std::cout << "couldn't m_resolve\n";
	}
}



void 
Client::enqueue(std::shared_ptr<Datapoint> dp) {
	DEBUGFUNC;
	m_pending.push_back(dp);		
}



void
Client::send() {
	DEBUGFUNC;
	boost::asio::ip::tcp::resolver::query q{m_host, m_port};
	m_resolv.async_resolve(q, boost::bind(&Client::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	m_ioservice.reset();
//	auto start = std::chrono::high_resolution_clock::now();
	m_ioservice.run();
//	auto elapsed = std::chrono::high_resolution_clock::now() - start;
//	long long delta = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//	std::cout << delta << " micro seconds" << std::endl;
	m_tcp_socket.close();
}
