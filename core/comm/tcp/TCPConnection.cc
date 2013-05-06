#include "TCPConnection.h"

TCPConnection::TCPConnection(asio::io_service &io_service)
   : Connection(io_service),
     socket_(io_service)
{
}


TCPConnection::TCPConnection(MessageHandler &server, 
			     asio::io_service &io_service)
   : TCPConnection(io_service)
{
	server_ = &server;
}


TCPConnection::TCPConnection(MessageHandler &server, 
			     asio::io_service &io_service, 
			     const std::string &address, 
			     const std::string &port)
   : Connection(io_service),
     socket_(io_service)
{
	server_ = &server;
	
	try {
		do_connection(address, port);
	} catch(TCPConnectionException &e) {
		throw;
	}
}


void TCPConnection::do_connection(const std::string &address, 
				  const std::string &port)
{
	asio::ip::tcp::resolver resolver(*io_service_);
	asio::ip::tcp::resolver::query query(address, port);
	asio::ip::tcp::resolver::iterator endpoint;
	
	try {
		endpoint = resolver.resolve(query);
	} catch(boost::system::system_error &e) {
		throw(new TCPConnectionException("Unable to resolve address."));
	}

	asio::async_connect(socket_, endpoint, 
			    boost::bind(&TCPConnection::handle_connect, this,
					asio::placeholders::error));
}


void TCPConnection::handle_connect(const boost::system::error_code &error)
{
	if (!error) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Connection successfully connected.");
#endif
		start();
		
// 		buffer_in_.resize(HEADER_SIZE);
// 		
// 		asio::async_read(socket_, asio::buffer(buffer_in_),
// 				asio::transfer_exactly(HEADER_SIZE),
// 				boost::bind(&TCPConnection::handle_read_header, 
// 					    this, asio::placeholders::error));
	}
}



void TCPConnection::handle_read_header(const boost::system::error_code &error) 
{
	if (!error) {
#ifdef NDEBUG
		syslog(LOG_NOTICE, "Message header read (%s).", 
			show_hex<std::vector<boost::uint8_t> >(buffer_in_).c_str());
#endif
		unsigned msglen = msg_.decode_header(buffer_in_);
		
		if (msglen > 0) {
			start_read_body(msglen);
		}  else {
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


void TCPConnection::start_read_body(unsigned msglen)
{
	buffer_in_.resize(HEADER_SIZE + msglen);
	asio::mutable_buffers_1 buf = asio::buffer(&buffer_in_[HEADER_SIZE], msglen);
	
	asio::async_read(socket_, buf,
		strand_.wrap(
			boost::bind(&TCPConnection::handle_read_body, this, 
				    asio::placeholders::error)));
}


void TCPConnection::handle_read_body(const boost::system::error_code &error)
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




void TCPConnection::handle_message()
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


void TCPConnection::start()
{
	buffer_in_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_in_),
			 asio::transfer_exactly(HEADER_SIZE),
			 boost::bind(&TCPConnection::handle_read_header, this,
				     asio::placeholders::error));
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Waiting for message.");
#endif
}


void TCPConnection::isend(const ConnectionMessage &msg)
{
	if (!msg.pack(buffer_out_)) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Error packing message.");
#endif
		return;
	}

#ifndef NDEBUG
	syslog(LOG_NOTICE, "Sending TCP message (%s).\n", 
	       show_hex<std::vector<boost::uint8_t> >(buffer_out_).c_str());
#endif
	asio::async_write(socket_, asio::buffer(buffer_out_), 
			  boost::bind(&TCPConnection::handle_write, this, 
				      asio::placeholders::error));
}


void TCPConnection::handle_write(const boost::system::error_code &e)
{
	if (!e) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Message successfully sent.");
#endif
	}
}


boost::asio::ip::tcp::socket &TCPConnection::socket()
{
	return socket_;
}


void TCPConnection::disconnect()
{
	socket_.close();
}



