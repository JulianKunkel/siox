#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <exception>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <core/comm/Connection.hpp>
#include <core/comm/ServiceServer.hpp>

namespace asio = boost::asio;

class TCPConnectionException 
   : public std::exception {
	   
public:
	TCPConnectionException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class TCPConnection 
   : public Connection {
	   
public:
	explicit TCPConnection(Service &service, 
			       asio::io_service &io_service);
	
	explicit TCPConnection(Service &service, 
			       asio::io_service &io_service, 
			       const std::string &address,
			       const std::string &port);

	void start();
	void isend(boost::shared_ptr<ConnectionMessage> msg);

	asio::ip::tcp::socket &socket();
private:
	std::string socket_address_, socket_port_;
	asio::ip::tcp::socket socket_;

	void do_connection();
	void do_disconnect();
	void start_read_body(unsigned msglen);
};

typedef boost::shared_ptr<TCPConnection> TCPConnection_ptr;

#endif

