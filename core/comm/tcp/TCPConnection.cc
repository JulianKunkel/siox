#include "TCPConnection.h"

TCPConnection::TCPConnection(asio::io_service &io_service)
   : Connection(io_service),
     socket_(io_service)
{
}


TCPConnection::TCPConnection(ServiceServer &server, 
			     asio::io_service &io_service)
   : TCPConnection(io_service)
{
	syslog(LOG_NOTICE, "Server::Creating TCP connection for server and io_service.");
	server_ = &server;
}


TCPConnection::TCPConnection(asio::io_service &io_service, 
			     const std::string &address, 
			     const std::string &port)
   : Connection(io_service),
     socket_(io_service)
{
	syslog(LOG_NOTICE, "Client::Creating TCP connection for client at %s:%s.", address.c_str(), port.c_str());
	
	asio::ip::tcp::resolver resolver(io_service);
	asio::ip::tcp::resolver::query query(address, port);
	asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(query);

	asio::async_connect(socket_, endpoint, 
			    boost::bind(&TCPConnection::handle_connect, this,
					asio::placeholders::error));
}


TCPConnection::TCPConnection(ServiceServer &server, 
			     asio::io_service &io_service, 
			     const std::string &address, 
			     const std::string &port)
   : TCPConnection(io_service, address, port)
{
	server_ = &server;
}


void TCPConnection::handle_connect(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "Client::Handling connection.");
	
	if (!error) {
		
		syslog(LOG_NOTICE, "Client::Connection connected.");
		
// 		asio::async_read(socket_, asio::buffer(buffer_),
// 				 boost::bind(&TCPConnection::handle_read_header,
// 					     this, asio::placeholders::error));
		
	}
}


void TCPConnection::start()
{
	syslog(LOG_NOTICE, "Server::start");
	
	buffer_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_),
			 asio::transfer_exactly(HEADER_SIZE),
			 boost::bind(&TCPConnection::handle_read_header,
				     shared_from_this(), 
				     asio::placeholders::error));
	
// 	asio::async_read(socket_, asio::buffer(buffer_), 
// 		boost::bind(&TCPConnection::handle_read_header,
// 			    shared_from_this(),
// 			    asio::placeholders::error));
}


void TCPConnection::handle_read_header(const boost::system::error_code &error) 
{
	syslog(LOG_NOTICE, "Server::Header read, length %d.", HEADER_SIZE);
	
	if (!error) {
		syslog(LOG_NOTICE, "Server::Header is %s.", show_hex<std::vector<boost::uint8_t> >(buffer_).c_str());
		unsigned msglen = msg_.decode_header(buffer_);
		start_read_body(msglen);
	} else {
		syslog(LOG_ERR, error.message().c_str());
	}
}


void TCPConnection::start_read_body(unsigned msglen)
{
	syslog(LOG_NOTICE, "TCP::start_read_body(hdrlen = %u, msglen = %u)", 
	       HEADER_SIZE, msglen);
	
	buffer_.resize(HEADER_SIZE + msglen);
	asio::mutable_buffers_1 buf = asio::buffer(&buffer_[HEADER_SIZE], 
						   msglen);
	
	asio::async_read(socket_, buf,
		strand_.wrap(
			boost::bind(&TCPConnection::handle_read_body, 
				    shared_from_this(), 
				    asio::placeholders::error)));
	
// 	asio::async_read(socket_, buf,
// 		boost::bind(&TCPConnection::handle_read_body, 
// 			    shared_from_this(), asio::placeholders::error));
}


void TCPConnection::handle_read_body(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "TCP::handle_read_body");
	
	if (!error) {
		handle_message();
		start();
	} else {
		syslog(LOG_ERR, "TCP::Error: %s", error.message().c_str());
	}
}


void TCPConnection::handle_message()
{
	syslog(LOG_NOTICE, "TCPConnection::handle_message");
	syslog(LOG_NOTICE, "TCP::Received buffer: %s", show_hex<std::vector<boost::uint8_t> >(buffer_).c_str());
	
	msg_.unpack(buffer_);

	server_->handle_message(msg_);
	
	syslog(LOG_NOTICE, "TCC::message unpacked");
	syslog(LOG_NOTICE, "TCC::Received message unid = %lud  aid=%lud", msg_.get_msg()->unid(), msg_.get_msg()->aid());
}


void TCPConnection::isend(const ConnectionMessage &msg)
{
	syslog(LOG_NOTICE, "TCP::isend");
	
// 	std::vector<boost::uint8_t> msgbuf;
	msg.pack(buffer_);

	syslog(LOG_NOTICE, "TCP::Sending: %s.\n", show_hex<std::vector<boost::uint8_t> >(buffer_).c_str());
	
	asio::async_write(socket_, asio::buffer(buffer_), 
		boost::bind(&TCPConnection::handle_write, this, 
			    asio::placeholders::error));

// 	get_io_service()->post(boost::bind(&TCPConnection::do_write, this, msg));
}


// void TCPConnection::do_write(const ConnectionMessage &msg)
// {
// 	std::vector<boost::uint8_t> msgbuf;
// 	msg.pack(msgbuf);
// 	
// 	asio::async_write(socket_, asio::buffer(msgbuf), 
// 		boost::bind(&TCPConnection::handle_write, this, 
// 			    asio::placeholders::error));
// }


void TCPConnection::handle_write(const boost::system::error_code &e)
{
	if (!e) {
		syslog(LOG_NOTICE, "Client::Message successfully written.");
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



