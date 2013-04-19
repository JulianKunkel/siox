#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <syslog.h>
#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"

namespace asio = boost::asio;

class TCPConnection 
   : public Connection,
     public boost::enable_shared_from_this<TCPConnection>
{
public:
	explicit TCPConnection(asio::io_service &io_service);
	explicit TCPConnection(asio::io_service &io_service, 
			       const std::string &address, 
			       const std::string &port);
	asio::ip::tcp::socket &socket();
	
	void start();
	
	void disconnect();
	void isend(const ConnectionMessage &msg);
	
	
private:
	asio::ip::tcp::socket socket_;
	
	void handle_connect(const boost::system::error_code &error);
	
	void start_read_body(unsigned msglen);
	void handle_read_header(const boost::system::error_code &error);
	void handle_read_body(const boost::system::error_code &error);
	void handle_message();
	
	void do_write(const ConnectionMessage &msg);
	void handle_write(const boost::system::error_code &e);

	void do_close();
};

typedef boost::shared_ptr<TCPConnection> TCPConnection_ptr;

#endif

