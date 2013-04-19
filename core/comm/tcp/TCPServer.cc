#include "TCPServer.h"

TCPServer::TCPServer(const std::string &address, const std::string &port,
		     const std::size_t worker_pool_size) 
	: ServiceServer(worker_pool_size),
	  acceptor_(io_service_), 
	  new_connection_()
{
	asio::ip::tcp::resolver resolver(io_service_);
	asio::ip::tcp::resolver::query query(address, port);
	asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	start_accept();
}


void TCPServer::start_accept()
{
	syslog(LOG_NOTICE, "Starting accept.");
	
	new_connection_.reset(new TCPConnection(io_service_));
	acceptor_.async_accept(new_connection_->socket(), 
			       boost::bind(&TCPServer::handle_accept, this, 
					   asio::placeholders::error));
}


void TCPServer::handle_accept(const boost::system::error_code &e)
{
	if (!e) {
		new_connection_->start();
	}

	start_accept();
}

