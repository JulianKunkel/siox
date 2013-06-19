#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "ServiceServer.h"
#include "TCPConnection.h"

namespace asio = boost::asio;


class TCPServerException 
   : public std::exception {
	   
public:
	TCPServerException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


class TCPServer 
   : public ServiceServer {
	   
public:
	explicit TCPServer(const std::string &address, const std::string &port,
			   std::size_t worker_pool_size); 
	
// 	void ipublish(boost::shared_ptr<ConnectionMessage> msg);

private:
	asio::ip::tcp::acceptor acceptor_;
	TCPConnection_ptr new_connection_;
	std::vector<TCPConnection_ptr> connections_;
	
	void start_accept();
	void handle_accept(const boost::system::error_code &e);
};

#endif

