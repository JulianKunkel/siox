#include "IPCConnection.h"

IPCConnection::IPCConnection(asio::io_service &io_service)
   : Connection(io_service),
     socket_(io_service)
{
}


IPCConnection::IPCConnection(ServiceServer &server, 
			     asio::io_service &io_service)
   : IPCConnection(io_service)
{
	syslog(LOG_NOTICE, "Server::Creating IPC connection for server and io_service.");
	server_ = &server;
}


IPCConnection::IPCConnection(asio::io_service &io_service, 
			     const std::string &path)
   : Connection(io_service),
     socket_(io_service)
{
	syslog(LOG_NOTICE, "Client::Creating IPC connection for client at %s.", path.c_str());
	
	asio::local::stream_protocol::socket socket(io_service);
	asio::local::stream_protocol::endpoint endpoint(path);

	socket_.async_connect(endpoint, 
			      boost::bind(&IPCConnection::handle_connect, this,
					  asio::placeholders::error));
	
}


IPCConnection::IPCConnection(ServiceServer &server, 
			     asio::io_service &io_service, 
			     const std::string &path)
   : IPCConnection(io_service, path)
{
	server_ = &server;
}
     
    
    
void IPCConnection::handle_connect(const boost::system::error_code &error)
{
	syslog(LOG_NOTICE, "Client::Handling connection.");
	
	if (!error) {
		
		syslog(LOG_NOTICE, "Client::Connection connected.");
		
// 		asio::async_read(socket_, asio::buffer(buffer_),
// 				 boost::bind(&IPCConnection::handle_read_header,
// 					     this, asio::placeholders::error));
		
	}
}


void IPCConnection::handle_read_header(const boost::system::error_code &error) 
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


void IPCConnection::start_read_body(unsigned msglen)
{
	syslog(LOG_NOTICE, "IPC::start_read_body(hdrlen = %u, msglen = %u)", 
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
	syslog(LOG_NOTICE, "IPC::handle_read_body");
	
	if (!error) {
		handle_message();
		start();
	} else {
		syslog(LOG_ERR, "IPC::Error: %s", error.message().c_str());
	}
}


void IPCConnection::handle_message()
{
	syslog(LOG_NOTICE, "IPC::handle_message");
	syslog(LOG_NOTICE, "IPC::Received buffer: %s", show_hex<std::vector<boost::uint8_t> >(buffer_).c_str());
	
	msg_.unpack(buffer_);

	server_->handle_message(msg_);
	
	syslog(LOG_NOTICE, "IPC::message unpacked");
	syslog(LOG_NOTICE, "IPC::Received message unid = %lud  aid=%lud type=%d", msg_.get_msg()->unid(), msg_.get_msg()->aid(), msg_.get_msg()->type());
}


void IPCConnection::start()
{
	syslog(LOG_NOTICE, "Server::start");
	
	buffer_.resize(HEADER_SIZE);
	
	asio::async_read(socket_, asio::buffer(buffer_),
			 asio::transfer_exactly(HEADER_SIZE),
			 boost::bind(&IPCConnection::handle_read_header, 
				     shared_from_this(), 
				     asio::placeholders::error));
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
	}
}


void IPCConnection::isend(const ConnectionMessage &msg)
{
	syslog(LOG_NOTICE, "IPC::isend");
	
// 	std::vector<boost::uint8_t> msgbuf;
	msg.pack(buffer_);

	syslog(LOG_NOTICE, "IPC::Sending: %s.\n", show_hex<std::vector<boost::uint8_t> >(buffer_).c_str());
	
	asio::async_write(socket_, asio::buffer(buffer_), 
		boost::bind(&IPCConnection::handle_write, this, 
			    asio::placeholders::error));
	
	// 	get_io_service()->post(boost::bind(&IPCConnection::do_write, this, msg));

}



asio::local::stream_protocol::socket &IPCConnection::socket()
{
	return socket_;
}


void IPCConnection::disconnect()
{
	socket_.close();
}


