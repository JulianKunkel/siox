#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#include <boost/asio.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include <core/comm/ServiceServer.hpp>
#include <core/comm/ipc/IPCConnection.hpp>

namespace asio = boost::asio;

class IPCServerException 
   : public std::exception {
	   
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
	
// 	void ipublish(boost::shared_ptr<ConnectionMessage> msg);

private:
	IPCConnection_ptr new_connection_;
	std::vector<IPCConnection_ptr> connections_;
	asio::local::stream_protocol::acceptor acceptor_;
	
	void start_accept();
	void handle_accept(const boost::system::error_code &error);
};

#endif
