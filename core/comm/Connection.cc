#include "Connection.h"

Connection::Connection(asio::io_service &io_service)
   : io_service_(&io_service),
     msg_(boost::shared_ptr<siox::MessageBuffer>(new siox::MessageBuffer()))
{
}

Connection::Connection(asio::io_service &io_service, Service &service) 
   : Connection(io_service)
{
	service_ = &service;
}


asio::io_service *Connection::get_io_service()
{
	return io_service_;
}


void Connection::handle_connect(const boost::system::error_code &e)
{
	if (!e) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Connection successfully connected.");
#endif
		start();
	}
}


void Connection::handle_read_header(const boost::system::error_code &e) 
{
	if (!e) {
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
			mtx_bufin_.unlock();
			start();
		}
		
	} else {
#ifndef NDEBUG
		syslog(LOG_ERR, "Error reading message header: %s", 
		       e.message().c_str());
#endif
		mtx_bufin_.unlock();
		start();
	}

}


void Connection::handle_read_body(const boost::system::error_code &e)
{
	if (!e) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Message body read.");
#endif
		handle_message();
		
	} else {
#ifndef NDEBUG
		syslog(LOG_ERR, "Error reading message body: %s", 
		       e.message().c_str());
#endif
	}
	
	mtx_bufin_.unlock();
	start();
}


void Connection::handle_write(const boost::system::error_code &e)
{
	if (!e) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Message successfully sent.");
#endif
	}
	
	mtx_bufout_.unlock();
}


void Connection::handle_write(boost::shared_ptr<ConnectionMessage> msg,
			      const boost::system::error_code &e)
{
	if (!e) {
#ifndef NDEBUG
		syslog(LOG_NOTICE, "Message successfully sent.");
#endif
	}
	
	mtx_bufout_.unlock();
}


void Connection::handle_message()
{
#ifndef NDEBUG
	syslog(LOG_NOTICE, "Received message (%s)", 
	       show_hex<std::vector<boost::uint8_t> >(buffer_in_).c_str());
#endif
	if (msg_.unpack(buffer_in_))
		service_->handle_message(msg_, *this);
#ifndef NDEBUG
	else 
		syslog(LOG_NOTICE, "Error unpacking message.");
#endif
	
}

void Connection::disconnect()
{
	do_disconnect();
}


void Connection::ireconnect()
{
	do_connection();
}
