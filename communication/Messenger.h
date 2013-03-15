/**
 * @file Messenger.h Messenger
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_SERVER_H
#define MESSAGE_SERVER_H

#include "MessageCallback.h"
#include "QueuedMessageConnection.h"
#include "WorkerPool.h"

#include <algorithm>
#include <list>

class MessageCallback;
class QueuedMessageConnection;
class URI;

class Listener;

/**
 * 
 */
class Messenger {
private:
	std::list<QueuedMessageConnection> connections;
	WorkerPool worker_pool;

public:
	Messenger();
	virtual ~Messenger();

	QueuedMessageConnection *create_connection(URI &uri);
	QueuedMessageConnection *accept_connection(Listener &listener);
	QueuedMessageConnection *close_connection(QueuedMessageConnection 
						  &connection);
	
	void poll_connections();
	void add_connection(QueuedMessageConnection &connection);
	void remove_connection(QueuedMessageConnection &connection);
	
	void add_message_callback(QueuedMessageConnection &connection,
				  MessageCallback &callback, int direction);
	void remove_message_callback(QueuedMessageConnection &connection,
				     MessageCallback &callback, int direction);

};


#endif // MESSAGE_SERVER_H
