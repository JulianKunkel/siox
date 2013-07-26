#include "IPCConnection.hpp"

IPCConnection::IPCConnection(Service &service, 
			     asio::io_service &io_service)
   : Connection(io_service, service), socket_(io_service)
{
}


IPCConnection::IPCConnection(Service &service, 
			     asio::io_service &io_service, 
			     const std::string &path)
   : Connection(io_service, service), socket_path_(path), socket_(io_service) 
{
	try {
		do_connection();
	} catch(IPCConnectionException &e) {
		throw;
	}
}


void IPCConnection::do_connection()
{
	asio::local::stream_protocol::socket socket(*io_service_);
	asio::local::stream_protocol::endpoint endpoint(socket_path_);

	socket_.async_connect(endpoint, 
			      boost::bind(&IPCConnection::handle_connect, this, 
					  asio::placeholders::error));
}


void IPCConnection::start()
{
	mtx_bufin_.lock();
	buffer_in_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_in_),
			 asio::transfer_exactly(HEADER_SIZE),
			 boost::bind(&IPCConnection::handle_read_header, this, 
				     asio::placeholders::error));
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Waiting for message.");
#endif
}


void IPCConnection::start_read_body(unsigned msglen)
{
	buffer_in_.resize(HEADER_SIZE + msglen);
	asio::mutable_buffers_1 buf = asio::buffer(&buffer_in_[HEADER_SIZE], 
						   msglen);
	
	asio::async_read(socket_, buf, 
			 boost::bind(&IPCConnection::handle_read_body, this, 
				     asio::placeholders::error));
}


void IPCConnection::isend(boost::shared_ptr<ConnectionMessage> msg)
{
	mtx_bufout_.lock();
	
	if (!msg->pack(buffer_out_)) {
#ifndef NDEBUG
		logger->log(Logger::ERR, "Error packing message.");
#endif
		mtx_bufout_.unlock();
		return;
	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Sending IPC message (%s).\n", 
	       show_hex<std::vector<boost::uint8_t> >(buffer_out_).c_str());
#endif
	asio::async_write(socket_, asio::buffer(buffer_out_), 
			  boost::bind(&IPCConnection::handle_write, this, msg, 
				      asio::placeholders::error));
}


asio::local::stream_protocol::socket &IPCConnection::socket()
{
	return socket_;
}


void IPCConnection::do_disconnect()
{
	socket_.close();
}


