/**
 * @file MessageCallback.h Message Callback
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_CALLBACK_H
#define MESSAGE_CALLBACK_H

class Message;
class QueuedMessageConnection;

/**
 * 
 */
class MessageCallback {
protected:
	int type;
	void *callback_function;

public:
	MessageCallback();
	virtual ~MessageCallback();

	void execute(Message &message, QueuedMessageConnection &connection);

	bool operator==(const MessageCallback &callback) const;
	bool operator!=(const MessageCallback &callback) const;

};

#endif // MESSAGE_CALLBACK_H
