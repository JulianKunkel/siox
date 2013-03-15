/**
 * @file IPCConnection.h IPC Connection
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H

#include "QueuedMessageConnection.h"

#include <string>

class URI;
class Listener;

/**
 * Queued message connection using datagramm-based Unix Domain Sockets.
 */
class IPCConnection : public QueuedMessageConnection {
protected:
	int sock;
	std::string sock_path;
public:
	using QueuedMessageConnection::operator==;
	using QueuedMessageConnection::operator!=;
	
	IPCConnection(const std::string address);
	IPCConnection(Listener &listener);
	~IPCConnection();
	
	void disconnect();
	void reconnect();
	void send();
	
};

#endif // IPC_CONNECTION_H
