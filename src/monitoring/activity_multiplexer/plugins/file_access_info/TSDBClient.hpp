
#ifndef  TSDBClient_INC
#define  TSDBClient_INC

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include "TSDBData.hpp"

#include <iostream>

#define DEBUGFUNC std::cout << "CALL -- " <<  __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;


std::string encode64(const std::string &val);
std::string make_http_request(const std::string& enc, const std::string& host, const std::string& port, const std::string& json);
std::string make_http_request(const std::string& enc, const std::string& host, const std::string& port, const DatapointSet& dps);

class TSDBClient {
	public:
		TSDBClient();
		void init(const std::string& host, const std::string& port, const std::string& username, const std::string& password);
		void enque(const Datapoint& dp);
		void send();
//		void close();

	private:
		void response_read_handler (const boost::system::error_code& ec, size_t bytes_transferred);
		void response_handler (const boost::system::error_code &ec);
		void connect_handler (const boost::system::error_code &ec);
		void resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator ep_it);

		boost::asio::io_service m_ioservice;
		boost::asio::streambuf m_response;
		tcp::socket m_tcp_socket;
		tcp::resolver m_resolv;
		std::deque<Datapoint> m_pending;
		
		std::string m_base64;
		std::string m_port;
		std::string m_host;
};

#endif   /* ----- #ifndef TSDBClient_INC  ----- */
