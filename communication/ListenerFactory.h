/**
 * @file ListenerFactory.h Listener Factory
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef LISTENER_FACTORY_H
#define LISTENER_FACTORY_H

#include "Listener.h"

#include <string>

class MessageServer;

/**
 * This namespace contains factory functions for creating appropriate listeners.
 */
namespace ListenerFactory {
	
	/**
	 * Creates a new listener on the given address.
	 * 
	 * @param server Message server
	 * @param listen_address Listener address, e.g. <i>ipc:///home/siox.socket</i> or <i>tcp://localhost:5000</i>.
	 */
	Listener *create_listener(MessageServer &server, 
				  const std::string listen_address);
};

#endif // LISTENER_FACTORY_H
