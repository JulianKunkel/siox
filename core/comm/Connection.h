#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>

#include "siox.pb.h"
#include "Callback.h"
#include "ConnectionMessage.h"

namespace asio = boost::asio;

class Connection 
	: private boost::noncopyable {
public:
	Connection(asio::io_service &io_service);
	
	void disconnect();
	void ireconnect();
	void isend(const ConnectionMessage &msg);
	void isend(const ConnectionMessage &msg, Callback &rsp_cb);
	void register_connection_callback(Callback &conn_cb);
	void register_message_callback(int msg_type, Callback &msg_cb);
	void subscribe(int msg_type, Callback &cb);
	asio::io_service *get_io_service();
protected:
	asio::io_service *io_service_;
	asio::io_service::strand strand_;
	std::vector<uint8_t> buffer_;
	ConnectionMessage msg_;
};

#endif
