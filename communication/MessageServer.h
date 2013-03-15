/**
 * @file MessageServer.h Message Server
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */
#ifndef MESSAGE_SERVER_H
#define MESSAGE_SERVER_H

#include "Listener.h"
#include "Messenger.h"

#include <algorithm>
#include <list>

/**
 * 
 */
class MessageServer  {
private:
	std::list<Listener> listeners;

public:
	MessageServer();
	~MessageServer();

	void add_listener(Listener &listener);
	void remove_listener(Listener &listener);
	void poll_listeners();
};

#endif // MESSAGE_SERVER_H
