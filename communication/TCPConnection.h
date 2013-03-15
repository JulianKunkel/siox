/**
 * @file TCPConnection.h TCP/IP Connection
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "QueuedMessageConnection.h"

#include <string>

class Listener;
class URI;

/**
 * 
 */
class TCPConnection : public QueuedMessageConnection {
public:
	using QueuedMessageConnection::operator==;
	using QueuedMessageConnection::operator!=;
	
	TCPConnection(URI &uri);
	TCPConnection(Listener &listener);
	~TCPConnection();
	
	void disconnect();
	void reconnect();
	void send();

};

#endif // TCP_CONNECTION_H
