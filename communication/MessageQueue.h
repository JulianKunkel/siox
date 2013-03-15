/**
 * @file MessageQueue.h Message Queue.
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "Message.h"

#include <queue>

class MessageCallback;
class MessageHandler;

/**
 * FIFO queue storing SIOX messages with the particularity that the dequeue()
 * method will trigger associated callbacks.
 */
class MessageQueue {
private:
	std::queue<Message> messages;
	bool is_blocked;
	MessageHandler *message_handler;
public:
	MessageQueue();
	virtual ~MessageQueue();

	void enqueue(const Message &message);
	Message *dequeue();

	/**
	 * Check to see if the queue is not empty.
	 */
	bool peek();

	void block(const int operation);
	void unblock(const int operation);
	
	void add_message_callback(const MessageCallback &callback);
	void remove_message_callback(const MessageCallback &callback);

};

#endif // MESSAGE_QUEUE_H
