/**
 * @file IPCListener.h IPC Listener
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef IPC_LISTENER_H
#define IPC_LISTENER_H

#include "Listener.h"

#include <string>

/**
 * Creates a datagramm Unix Domain Socket and binds to it.
 */
class IPCListener : public Listener {
private:
	int sock;
	std::string sock_path;
public:
	using Listener::operator==;
	using Listener::operator!=;

	/**
	 * Creates a Unix Domain Socket at the given path and binds it.
	 */
	IPCListener(const std::string path);
	IPCListener(Listener &listener);
	/**
	 * Closes and removes the socket.
	 */
	~IPCListener();
	
	std::string get_address();
	int get_bound_socket();
};

#endif // IPC_LISTENER_H
