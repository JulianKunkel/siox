#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

#include <siox.pb.h>
#include <core/comm/Callback.hpp>
#include <core/comm/ConnectionMessage.hpp>
#include <core/comm/Service.hpp>
#include <core/logger/SioxLogger.hpp>

namespace asio = boost::asio;

class Connection 
   : public boost::enable_shared_from_this<Connection>,
     private boost::noncopyable {
	     
public:
	explicit Connection(asio::io_service &io_service);
	explicit Connection(asio::io_service &io_service, Service &service);
	
	void disconnect();
	void ireconnect();
	asio::io_service *get_io_service();
	
	virtual void start() = 0;
	virtual void isend(boost::shared_ptr<ConnectionMessage> msg) = 0;
	virtual void start_read_body(unsigned msglen) = 0;
	
protected:
	Service *service_;
	asio::io_service *io_service_;
	std::vector<uint8_t> buffer_out_;
	std::vector<uint8_t> buffer_in_;
	boost::mutex mtx_bufout_, mtx_bufin_;
	ConnectionMessage msg_;
	
	virtual void do_connection() = 0;
	virtual void do_disconnect() = 0;
	
	void handle_connect(const boost::system::error_code &e);
	void handle_read_header(const boost::system::error_code &e); 
	void handle_read_body(const boost::system::error_code &e);
	void handle_write(const boost::system::error_code &e);
	void handle_write(boost::shared_ptr<ConnectionMessage> msg,
			  const boost::system::error_code &e);

	void handle_message();
};

#endif
