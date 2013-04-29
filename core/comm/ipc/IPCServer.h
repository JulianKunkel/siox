#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#include <boost/asio.hpp>
#include <syslog.h>

#include "ServiceServer.h"
#include "IPCConnection.h"

namespace asio = boost::asio;

class IPCServer 
   : public ServiceServer {
	   
public:
	explicit IPCServer(const std::string &path, 
			   std::size_t worker_pool_size);
		
private:
	asio::local::stream_protocol::acceptor acceptor_;
	IPCConnection_ptr new_connection_;
	
	void start_accept();
	void handle_accept(const boost::system::error_code &error);
};

#endif
