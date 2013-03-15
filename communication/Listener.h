/**
 * @file Listener.h Listener
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef LISTENER_H
#define LISTENER_H

#include <string>

class MessageServer;

/**
 * Provides the interface to be implemented by concrete listeners like 
 * IPC & TCP listeners.
 * 
 * @see IPCListener
 * @see TCPListener
 */
class Listener {
public:
	Listener(MessageServer &server, const std::string listen_address);
	Listener();
	virtual ~Listener();
	
	virtual std::string get_address();
	bool operator==(const Listener &listener) const;
	bool operator!=(const Listener &listener) const;
};

#endif // LISTENER_H
