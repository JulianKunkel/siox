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

#include "Callback.h"
#include "siox.pb.h"
#include "ConnectionMessage.h"

class Server : private boost::noncopyable {
public:
	/**
	 * Advertise that a new message type is available.
	 */
	virtual void advertise(siox::MessageBuffer::MessageType message_type) 
		= 0;
	
	/**
	 * Pushed a message to all subscribed clients.
	 */
	virtual void ipublish(ConnectionMessage &message) = 0;
	
	/**
	 */
	virtual void isend_response(ConnectionMessage &message,
				    ConnectionMessage &response) = 0;
	
	/**
	 */
	virtual void register_response_callback(
		siox::MessageBuffer::MessageType type, 
		Callback &message_received_callback) = 0;
};

#endif

