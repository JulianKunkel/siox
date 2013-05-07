#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H

#include <exception>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"
#include "ServiceServer.h"

#define IPC_SOCKET_PATH "/tmp/siox.socket"

namespace asio = boost::asio;


class IPCConnectionException : public std::exception {
public:
	IPCConnectionException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class IPCConnection 
   : public Connection, 
     public boost::enable_shared_from_this<IPCConnection>
{
public:
	explicit IPCConnection(asio::io_service &io_service);
	
	explicit IPCConnection(MessageHandler &server, 
			       asio::io_service &io_service);

	explicit IPCConnection(MessageHandler &server, 
			       asio::io_service &io_service, 
			       const std::string &path);

	asio::local::stream_protocol::socket &socket();
	
	void start();
	
	void disconnect();
	
	void isend(const ConnectionMessage &msg);
	
private:
	asio::local::stream_protocol::socket socket_;
	MessageHandler *server_;
	
	void do_connection(const std::string &path);
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

