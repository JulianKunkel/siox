#ifndef SERVICE_SERVER_H
#define SERVICE_SERVER_H

#include <list>
#include <map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "Callback.h"
#include "Connection.h"
#include "Service.h"
#include "ServiceServer.h"
#include "SioxLogger.h"

namespace asio = boost::asio;

class ServiceServer 
   : public Service,
     private boost::noncopyable {
	   
public:
	ServiceServer(const std::size_t worker_pool_size);
	
	/** Creates the workers and attaches them to the io_service. */
	void run();
	
	/** Stops all workers */
	void stop();

	/** Advertises a new message type among all connected clients. */
	void advertise(boost::uint64_t mtype);
	
	/** Sends the message to all clients who subscribed the message's type */
	void ipublish(boost::shared_ptr<ConnectionMessage> msg);
	
	void register_message_callback(Callback &msg_rcvd_callback);
	void clear_message_callbacks();
	
	void register_error_callback(Callback &err_callback);
	void clear_error_callbacks();
	
	void handle_message(ConnectionMessage &msg, 
			    Connection &connection);
	
	void handle_message(boost::shared_ptr<ConnectionMessage> msg, 
			    Connection &connection);
	
protected:
	asio::io_service io_service_;
	
	void handle_stop();
	
private:
	std::size_t worker_pool_size_;
	boost::ptr_list<Callback> error_callbacks_;
	boost::ptr_list<Callback> message_callbacks_;
	std::vector<boost::shared_ptr<boost::thread> > workers_;
	std::multimap<boost::uint32_t, Connection *> subscribed_connections;
	
	void subscribe(boost::uint32_t mtype, Connection &connection);
};

#endif

