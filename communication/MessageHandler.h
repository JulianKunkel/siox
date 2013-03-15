/**
 * @file MessageHandler.h Message Handler
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "MessageCallback.h"

#include <algorithm>
#include <list>

class Message;
class QueuedMessageConnection;

/**
 * A message handler has a set of message callbacks that can be executed on a 
 * given message.
 */
class MessageHandler {
private:
	std::list<MessageCallback> message_callbacks;
public:
	MessageHandler();
	~MessageHandler();
	
	void register_callback(const MessageCallback &callback);
	void unregister_callback(const MessageCallback &callback);
	void execute_callback(Message &message, 
			      QueuedMessageConnection &connection);
};

#endif
