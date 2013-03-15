/**
 * @file MessageClient.h Message Client
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_CLIENT_H
#define MESSAGE_CLIENT_H

#include "Messenger.h"

#include <string>

class Message;
class QueuedMessageConnection;

/**
 * 
 * 
 */
class MessageClient : Messenger {
public:
	MessageClient();
	~MessageClient();
	
	void send(Message &message, QueuedMessageConnection &connection);
	void queued_send(Message &message, QueuedMessageConnection &connection);
};

#endif // MESSAGE_CLIENT_H
