#include "IPCConnection.h"

IPCConnection::IPCConnection(asio::io_service &io_service)
   : Connection(io_service),
     socket_(io_service)
{
}


IPCConnection::IPCConnection(MessageHandler &server, 
			     asio::io_service &io_service)
   : IPCConnection(io_service)
{
	server_ = &server;
}


IPCConnection::IPCConnection(MessageHandler &server, 
			     asio::io_service &io_service, 
			     const std::string &path)
   : Connection(io_service),
     socket_(io_service)
{
	server_ = &server;
	
	try {
		do_connection(path);
	} catch(IPCConnectionException &e) {
		throw;
	}
}

void IPCConnection::do_connection(const std::string &path)
{
	asio::local::stream_protocol::socket socket(*io_service_);
	asio::local::stream_protocol::endpoint endpoint(path);

	socket_.async_connect(endpoint, 
			      boost::bind(&IPCConnection::handle_connect, this,
					  asio::placeholders::error));
}
    
    
void IPCConnection::handle_connect(const boost::system::error_code &error)
{
	if (!error) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Connection successfully connected.");
#endif
		start();
		
// 		asio::async_read(socket_, asio::buffer(buffer_),
// 				 boost::bind(&IPCConnection::handle_read_header,
// 					     this, asio::placeholders::error));
		
	}
}


void IPCConnection::handle_read_header(const boost::system::error_code &error) 
{
	if (!error) {
#ifdef NDEBUG
		syslog(LOG_NOTICE, "Message header read (%s).", 
			show_hex<std::vector<boost::uint8_t> >(buffer_in_).c_str());
#endif
		unsigned msglen = msg_.decode_header(buffer_in_);
		
		if (msglen > 0) {
			start_read_body(msglen);
			
		} else {
#ifndef NDEBUG
			syslog(LOG_NOTICE, 
			       "Discarding malformed message header.");
#endif
			start();
		}
		
	} else {
#ifndef NDEBUG
		syslog(LOG_ERR, "Error reading message header: %s", 
		       error.message().c_str());
#endif
	}

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


void IPCConnection::handle_read_body(const boost::system::error_code &error)
{
	if (!error) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Message body read.");
#endif
		handle_message();
		start();
		
	} else {
#ifndef NDEBUG
		syslog(LOG_ERR, "Error reading message body: %s", 
		       error.message().c_str());
#endif
	}
}


void IPCConnection::handle_message()
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Received message (%s)", 
	       show_hex<std::vector<boost::uint8_t> >(buffer_in_).c_str());
#endif
	if (msg_.unpack(buffer_in_))
		server_->handle_message(msg_);
#ifndef NDEBUG
	else 
		syslog(LOG_NOTICE, "Error unpacking message.");
#endif
	
}


void IPCConnection::start()
{
	buffer_in_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_in_),
			 asio::transfer_exactly(HEADER_SIZE),
			 boost::bind(&IPCConnection::handle_read_header, this, 
				     asio::placeholders::error));
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Waiting for message.");
#endif
	
}


void IPCConnection::handle_read(const boost::system::error_code &e, 
				 std::size_t bytes_transferred)
{
}


void IPCConnection::handle_write(const boost::system::error_code &e)
{
	std::cout << "Handling write..." << std::endl;
	
	if (!e) {
		boost::system::error_code ignored_ec;
	}
}


void IPCConnection::isend(const ConnectionMessage &msg)
{
	if (!msg.pack(buffer_out_)) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Error packing message."); 
#endif
		return;
	}

#ifndef NDEBUG
	syslog(LOG_NOTICE, "Sending IPC message (%s).\n", 
	       show_hex<std::vector<boost::uint8_t> >(buffer_out_).c_str());
#endif
	
	asio::async_write(socket_, asio::buffer(buffer_out_),
			  boost::bind(&IPCConnection::handle_write, this,
				      asio::placeholders::error));
}



asio::local::stream_protocol::socket &IPCConnection::socket()
{
	return socket_;
}


void IPCConnection::disconnect()
{
	socket_.close();
}


