/**
 * @file QueuedMessageConnection.h Queued Message Connection
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef QUEUED_MESSAGE_CONNECTION_H
#define QUEUED_MESSAGE_CONNECTION_H

#include "URI.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <string>

class Message;
class MessageCallback;
class MessageQueue;

/**
 * 
 */
class QueuedMessageConnection {
protected:
	URI *remote_address;
	MessageQueue *message_queue_in;
	MessageQueue *message_queue_out;
	
public:
	QueuedMessageConnection(const std::string address);
	QueuedMessageConnection();
	virtual ~QueuedMessageConnection();
	
	virtual void disconnect();
	virtual void reconnect();
	virtual void send(Message &message);
	void queued_send(Message &message);
	Message *recv();
	
	void add_message_callback(MessageCallback &callback, int direction);
	void remove_message_callback(MessageCallback &callback, int direction);

	bool operator==(const QueuedMessageConnection &conn) const;
	bool operator!=(const QueuedMessageConnection &conn) const;

};

#endif // MESSAGE_CONNECTION_H
