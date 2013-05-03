#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#include <boost/asio.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <syslog.h>

#include "ServiceServer.h"
#include "IPCConnection.h"

namespace asio = boost::asio;

class IPCServerException : public std::exception {
public:
	IPCServerException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class IPCServer 
   : public ServiceServer {
	   
public:
	explicit IPCServer(const std::string &path, 
			   std::size_t worker_pool_size);
		
private:
	asio::local::stream_protocol::acceptor acceptor_;
	IPCConnection_ptr new_connection_;
	boost::ptr_list<asio::local::stream_protocol::socket> connected_sockets_;
	
	void start_accept();
	void handle_accept(const boost::system::error_code &error);
};

#endif
