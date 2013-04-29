#include "IPCServer.h"

IPCServer::IPCServer(const std::string &path, 
		     const std::size_t worker_pool_size)
	: ServiceServer(worker_pool_size),
	  acceptor_(io_service_),
	  new_connection_()
	  
{
	unlink(path.c_str());
	
	asio::local::stream_protocol::endpoint endpoint(path);
	
	acceptor_.open(endpoint.protocol());
	acceptor_.bind(endpoint);
	acceptor_.listen();

	syslog(LOG_NOTICE, "Server::Constructor created and bound IPC server.");
	
	start_accept();
}


void IPCServer::start_accept()
{
	syslog(LOG_NOTICE, "Server::Starting accept.");
	
	new_connection_.reset(new IPCConnection(*this, io_service_));
	acceptor_.async_accept(new_connection_->socket(), 
	       boost::bind(&IPCServer::handle_accept, this, 
			   asio::placeholders::error));

}


void IPCServer::handle_accept(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "Server::Handling accepts.");
	
	if (!error) {
		syslog(LOG_NOTICE, "Server::New connection accepted.");
		new_connection_->start();
	}

	start_accept();
}


