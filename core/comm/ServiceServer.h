#ifndef BASE_SERVER_H
#define BASE_SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "Server.h"

namespace asio = boost::asio;

class ServiceServer : private Server {
public:
	ServiceServer(const std::size_t worker_pool_size);
	void run();
	
	void advertise(siox::MessageBuffer::MessageType message_type);
	
	void ipublish(ConnectionMessage &message);
	
	void isend_response(ConnectionMessage &message, 
			    ConnectionMessage &response);
				    
	void register_response_callback(siox::MessageBuffer::MessageType type, 
					Callback &message_received_callback);

protected:
	asio::io_service io_service_;
	void handle_stop();
	
private:
	asio::signal_set signals_;
	std::size_t worker_pool_size_;
};

#endif

