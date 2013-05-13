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
#include "Service.h"
#include "ServiceServer.h"

namespace asio = boost::asio;

class ServiceServer 
   : public Service,
     private boost::noncopyable {
	   
public:
	ServiceServer(const std::size_t worker_pool_size);
	
	void run();
	void stop();
	
	void advertise(boost::uint64_t mtype);
	
	virtual void ipublish(boost::shared_ptr<ConnectionMessage> msg) = 0;
	
	void isend_response(ConnectionMessage &msg, ConnectionMessage &rsp);
	void isend_response(boost::shared_ptr<ConnectionMessage> msg, 
			    boost::shared_ptr<ConnectionMessage> rsp);
				    
	void register_message_callback(Callback &msg_rcvd_callback);
	void clear_message_callbacks();
	void register_error_callback(Callback &err_callback);
	void clear_error_callbacks();
	
	void handle_message(ConnectionMessage &msg);
	void handle_message(boost::shared_ptr<ConnectionMessage> msg);
	
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

