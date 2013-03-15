/**
 * @file TCPListener.h TCP/IP Listener
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include <string>

#include "Listener.h"

/**
 * Creates a TCP/IP socket and binds to it.
 */
class TCPListener : public Listener {
public:
	using Listener::operator==;
	using Listener::operator!=;

	TCPListener();
	~TCPListener();
	
	std::string get_address();
	
};

#endif // TCP_LISTENER_H
