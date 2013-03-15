#include "MessageHandler.h"

MessageHandler::MessageHandler()
{
}


MessageHandler::~MessageHandler()
{
}
	

void MessageHandler::register_callback(const MessageCallback &callback)
{
}


void MessageHandler::unregister_callback(const MessageCallback &callback)
{
}


void MessageHandler::execute_callback(Message &message, 
				      QueuedMessageConnection &connection)
{
}

