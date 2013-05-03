#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <exception>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"
#include "ServiceServer.h"

namespace asio = boost::asio;

class TCPConnectionException : public std::exception {
public:
	TCPConnectionException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class TCPConnection 
   : public Connection,
     public boost::enable_shared_from_this<TCPConnection>
{
public:
	explicit TCPConnection(asio::io_service &io_service);
	
	explicit TCPConnection(MessageHandler &server, 
			       asio::io_service &io_service);
	
	explicit TCPConnection(asio::io_service &io_service, 
			       const std::string &address, 
			       const std::string &port);
	
	explicit TCPConnection(MessageHandler &server, 
			       asio::io_service &io_service, 
			       const std::string &address,
			       const std::string &port);

	asio::ip::tcp::socket &socket();
	
	void start();
	
	void disconnect();
	
	void isend(const ConnectionMessage &msg);
	void isend(asio::ip::tcp::socket sock, std::vector<uint8_t> buffer);
	
private:
	asio::ip::tcp::socket socket_;
	MessageHandler *server_;
	
	void handle_connect(const boost::system::error_code &error);
	void start_read_body(unsigned msglen);
	void handle_read_header(const boost::system::error_code &error);
	void handle_read_body(const boost::system::error_code &error);
	void handle_message();
	void handle_write(const boost::system::error_code &e);

};

typedef boost::shared_ptr<TCPConnection> TCPConnection_ptr;

#endif

