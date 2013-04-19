#include "IPCConnection.h"

IPCConnection::IPCConnection(asio::io_service &io_service)
	: Connection(io_service),
	  socket_(io_service)
{
}


IPCConnection::IPCConnection(asio::io_service &io_service, 
			     const std::string &path)
   : Connection(io_service),
     socket_(io_service)
{
	asio::local::stream_protocol::socket socket(io_service);
	asio::local::stream_protocol::endpoint endpoint(path);

	socket_.async_connect(endpoint, 
			      boost::bind(&IPCConnection::handle_connect, this,
					  asio::placeholders::error));
	
}


void IPCConnection::handle_connect(const boost::system::error_code &error)
{
	if (!error) {
		
		asio::async_read(socket_, asio::buffer(buffer_),
				 boost::bind(&IPCConnection::handle_read_header,
					     this, asio::placeholders::error));
		
	}
}


asio::local::stream_protocol::socket &IPCConnection::socket()
{
	return socket_;
}


void IPCConnection::start()
{
	syslog(LOG_NOTICE, "start");
	
	asio::async_read(socket_, asio::buffer(buffer_),
		boost::bind(&IPCConnection::handle_read_header, 
			    shared_from_this(), asio::placeholders::error));
}


void IPCConnection::handle_read_header(const boost::system::error_code &error) 
{
	syslog(LOG_NOTICE, "handler_read_header");
	
	if (!error) {
		syslog(LOG_NOTICE, "Got header");
		unsigned msglen = msg_.decode_header(buffer_);
		start_read_body(msglen);
	} else {
		syslog(LOG_ERR, error.message().c_str());
	}

}


void IPCConnection::start_read_body(unsigned msglen)
{
	syslog(LOG_NOTICE, "start_read_body(hdrlen = %u, msglen = %u)", 
	       HEADER_SIZE, msglen);
	
	buffer_.resize(HEADER_SIZE + msglen);
	asio::mutable_buffers_1 buf = asio::buffer(&buffer_[HEADER_SIZE], 
						   msglen);
	asio::async_read(socket_, buf,
		boost::bind(&IPCConnection::handle_read_body, 
			    shared_from_this(), asio::placeholders::error));
}


void IPCConnection::handle_read_body(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "handle_read_body");
	
	if (!error) {
		handle_message();
		start();
	} else {
		syslog(LOG_ERR, "Error: %s", error.message().c_str());
	}
}


void IPCConnection::handle_message()
{
	syslog(LOG_NOTICE, "handle_message");
	
	msg_.unpack(buffer_);

	syslog(LOG_NOTICE, "message unpacked");
	syslog(LOG_NOTICE, "Received message unid = %lud  aid=%lud", msg_.get_msg()->unid(), msg_.get_msg()->aid());
}


void IPCConnection::handle_read(const boost::system::error_code &e, 
				 std::size_t bytes_transferred)
{
	syslog(LOG_NOTICE, "Reading data");
}


void IPCConnection::handle_write(const boost::system::error_code &e)
{
	std::cout << "Handling write..." << std::endl;
	
	if (!e) {
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, 
				 ignored_ec);
	}
}
