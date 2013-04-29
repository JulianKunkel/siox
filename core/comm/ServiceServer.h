#ifndef BASE_SERVER_H
#define BASE_SERVER_H

#include <list>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "Callback.h"
#include "ServiceServer.h"

namespace asio = boost::asio;

class ServiceServer 
   : private boost::noncopyable {
	   
public:
	ServiceServer(const std::size_t worker_pool_size);
	
	void run();
	void stop();
	
	void advertise(siox::MessageBuffer::MessageType message_type);
	
	void ipublish(ConnectionMessage &message);
	
	void isend_response(ConnectionMessage &message, 
			    ConnectionMessage &response);
				    
	void register_response_callback(Callback
					&message_received_callback);
	
	void handle_message(ConnectionMessage &msg);
	
protected:
	asio::io_service io_service_;
	void handle_stop();
	
private:
	asio::signal_set signals_;
	std::size_t worker_pool_size_;
	std::vector<boost::shared_ptr<boost::thread> > workers;
	boost::ptr_list<Callback> response_callbacks;
};

#endif

