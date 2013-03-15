/**
 * @file ConnectionFactory.h Connection Factory
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef CONNECTION_FACTORY_H
#define CONNECTION_FACTORY_H

class Listener;
class URI;
class QueuedMessageConnection;

/**
 * This namespace contains factory functions for creating appropriate 
 * connections.
 */
namespace ConnectionFactory {
	
	/**
	 * 
	 */
	QueuedMessageConnection *create_connection(URI &remote_address);
	
	/**
	 * 
	 */
	QueuedMessageConnection *accept_connection(Listener &l);
};

#endif
