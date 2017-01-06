/*
 * =====================================================================================
 *
 *       Filename:  BaseClient.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2017 02:43:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  BaseClient_INC
#define  BaseClient_INC


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/algorithm/string.hpp>

#include "../Debug.hpp"
#include "Basics.hpp"

#include <stdint.h>
#include <string>
#include <memory>
#include <chrono>

using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;

class Client {
	public:
		using LongType = uint64_t; // 64bit are expected by ElasticSearch long type
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		struct Datapoint {
				LongType m_duration;
				LongType m_bytes;
				LongType m_calls;
				std::string m_access;
				std::string m_filename;
				TimePoint m_timestamp;
				std::string m_host;
				LongType m_jobid;
				LongType m_procid;
				std::string m_username;
		};

		Client();
		virtual void init(const std::string& host, const std::string& port, const std::string& username, const std::string& password);
		virtual void enqueue(std::shared_ptr<Datapoint> dp);
		virtual void send();
		virtual std::string to_json(std::shared_ptr<Client::Datapoint> point) const = 0;
//		void close(); // TODO

//	protected:
	public:
		void response_read_handler (const boost::system::error_code& ec, size_t bytes_transferred);
		void response_handler (const boost::system::error_code &ec);
		void connect_handler (const boost::system::error_code &ec);
		void resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator ep_it);

		virtual std::string make_send_uri() = 0;

		boost::asio::io_service m_ioservice;
		boost::asio::streambuf m_response;
		tcp::socket m_tcp_socket;
		tcp::resolver m_resolv;
		std::deque<std::shared_ptr<Client::Datapoint>> m_pending;
		
		std::string m_base64;
		std::string m_port;
		std::string m_host;
};


#endif   /* ----- #ifndef BaseClient_INC  ----- */
