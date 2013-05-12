#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "ConnectionMessage.h"

class MessageHandler {
public:
	virtual void handle_message(ConnectionMessage &msg) = 0;
};

#endif