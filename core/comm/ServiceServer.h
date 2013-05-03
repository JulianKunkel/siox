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
#include "MessageHandler.h"
#include "ServiceServer.h"

namespace asio = boost::asio;

class ServiceServer 
   : public MessageHandler,
     private boost::noncopyable {
	   
public:
	ServiceServer(const std::size_t worker_pool_size);
	
	void run();
	void stop();
	
	void advertise(siox::MessageBuffer::MessageType message_type);
	
	void ipublish(ConnectionMessage &message);
	
	void isend_response(ConnectionMessage &message, 
			    ConnectionMessage &response);
				    
	void register_message_callback(Callback
					&message_received_callback);
	void clear_message_callbacks();
	void register_error_callback(Callback
				     &error_callback);
	void clear_error_callbacks();
	
	void handle_message(ConnectionMessage &msg);
	
protected:
	asio::io_service io_service_;
	void handle_stop();
	
private:
	std::size_t worker_pool_size_;
	boost::ptr_list<Callback> error_callbacks_;
	boost::ptr_list<Callback> message_callbacks_;
	std::vector<boost::shared_ptr<boost::thread> > workers_;
};

#endif

