#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H

#include <syslog.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"
#include "Server.h"

namespace asio = boost::asio;

class IPCConnection 
	: public Connection, 
	  public boost::enable_shared_from_this<IPCConnection>
{
public:
	explicit IPCConnection(asio::io_service &io_service);
	explicit IPCConnection(asio::io_service &io_service, 
			       const std::string &path);

	asio::local::stream_protocol::socket &socket();
	void start();
	
private:
	asio::local::stream_protocol::socket socket_;
	
	void handle_connect(const boost::system::error_code &error);
	void start_read_body(unsigned msglen);
	void handle_read_header(const boost::system::error_code &error);
	void handle_read_body(const boost::system::error_code &error);
	void handle_read(const boost::system::error_code &e, 
			 std::size_t bytes_transferred);
	void handle_write(const boost::system::error_code &e);
	void handle_message();
};

typedef boost::shared_ptr<IPCConnection> IPCConnection_ptr;

#endif

