#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H

#include <exception>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <core/comm/Connection.hpp>
#include <core/comm/ServiceServer.hpp>

#define IPC_SOCKET_PATH "/tmp/siox.socket"

namespace asio = boost::asio;


class IPCConnectionException 
   : public std::exception {
	   
public:
	IPCConnectionException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class IPCConnection 
   : public Connection {
	   
public:
	explicit IPCConnection(Service &service, 
			       asio::io_service &io_service);
	explicit IPCConnection(Service &service, 
			       asio::io_service &io_service, 
			       const std::string &path);

	void start();
	void isend(boost::shared_ptr<ConnectionMessage> msg);
	asio::local::stream_protocol::socket &socket();
	
private:
	std::string socket_path_;
	asio::local::stream_protocol::socket socket_;
	
	void do_connection();
	void do_disconnect();
	void start_read_body(unsigned msglen);
};

typedef boost::shared_ptr<IPCConnection> IPCConnection_ptr;

#endif

