#ifndef SERVER_H
#define SERVER_H

/**
 * @file Server.h
 * 
 * The pure virtual class Server provides an interface for the network services
 * used by a SIOX service.
 * 
 * @author Alvaro Aguilera 
 */

#include <boost/noncopyable.hpp>

#include "siox.pb.h"
#include "Callback.h"
#include "ConnectionMessage.h"
#include "Service.h"

class Server : private boost::noncopyable {
public:
	virtual void run() = 0;
	virtual void stop() = 0;
	
	/**
	 * Advertise that a new message type is available.
	 */
// 	virtual void advertise(siox::MessageBuffer::MessageType message_type) 
// 		= 0;
	
	/**
	 * Pushed a message to all subscribed clients.
	 */
// 	virtual void ipublish(ConnectionMessage &message) = 0;
	
	/**
	 */
// 	virtual void isend_response(ConnectionMessage &message,
// 				    ConnectionMessage &response) = 0;
	
	/**
	 */
// 	virtual void register_response_callback(Callback<Service> 
// 						&message_received_callback) = 0;
};

#endif

