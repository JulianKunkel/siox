#include "TCPConnection.h"

TCPConnection::TCPConnection(asio::io_service &io_service)
   : Connection(io_service),
     socket_(io_service)
{
}


TCPConnection::TCPConnection(asio::io_service &io_service, 
			     const std::string &address, 
			     const std::string &port)
   : Connection(io_service),
     socket_(io_service)
{
	asio::ip::tcp::resolver resolver(io_service);
	asio::ip::tcp::resolver::query query(address, port);
	asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(query);

	asio::async_connect(socket_, endpoint, 
			    boost::bind(&TCPConnection::handle_connect, this,
					asio::placeholders::error));
	
}


void TCPConnection::handle_connect(const boost::system::error_code &error)
{
	if (!error) {
		
		asio::async_read(socket_, asio::buffer(buffer_),
				 boost::bind(&TCPConnection::handle_read_header,
					     this, asio::placeholders::error));
		
	}
}


void TCPConnection::handle_read_header(const boost::system::error_code &error) 
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


void TCPConnection::start_read_body(unsigned msglen)
{
	syslog(LOG_NOTICE, "start_read_body(hdrlen = %u, msglen = %u)", 
	       HEADER_SIZE, msglen);
	
	buffer_.resize(HEADER_SIZE + msglen);
	asio::mutable_buffers_1 buf = asio::buffer(&buffer_[HEADER_SIZE], 
						   msglen);
	asio::async_read(socket_, buf,
		boost::bind(&TCPConnection::handle_read_body, 
			    shared_from_this(), asio::placeholders::error));
}


void TCPConnection::handle_read_body(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "handle_read_body");
	
	if (!error) {
		handle_message();
		start();
	} else {
		syslog(LOG_ERR, "Error: %s", error.message().c_str());
	}
}


void TCPConnection::handle_message()
{
	syslog(LOG_NOTICE, "handle_message");
	
	msg_.unpack(buffer_);

	syslog(LOG_NOTICE, "message unpacked");
	syslog(LOG_NOTICE, "Received message unid = %lud  aid=%lud", msg_.get_msg()->unid(), msg_.get_msg()->aid());
}


void TCPConnection::start()
{
	syslog(LOG_NOTICE, "start");
	
	buffer_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_), 
		boost::bind(&TCPConnection::handle_read_header,
			    shared_from_this(),
			    asio::placeholders::error));
}


void TCPConnection::isend(const ConnectionMessage &msg)
{
	get_io_service()->post(boost::bind(&TCPConnection::do_write, this, msg));
}


void TCPConnection::do_write(const ConnectionMessage &msg)
{
	std::vector<boost::uint8_t> msgbuf;
	msg.pack(msgbuf);
	
	asio::async_write(socket_, asio::buffer(msgbuf), 
		boost::bind(&TCPConnection::handle_write, this, 
			    asio::placeholders::error));
}


void TCPConnection::handle_write(const boost::system::error_code &e)
{
	if (!e) {
		std::cout << "Message successfully written..." << std::endl;
	}
}


boost::asio::ip::tcp::socket &TCPConnection::socket()
{
	return socket_;
}


void TCPConnection::disconnect()
{
	get_io_service()->post(boost::bind(&TCPConnection::do_close, this));
}


void TCPConnection::do_close()
{
	socket_.close();
}

