#include "MessageCallback.h"

MessageCallback::MessageCallback() 
{ 
	
}


MessageCallback::~MessageCallback() 
{ 

}


void MessageCallback::execute(Message &message, QueuedMessageConnection &connection)
{
	
}


bool MessageCallback::operator==(const MessageCallback &callback) const
{
	if (this == &callback)
		return true;
	
	return false;
}


bool MessageCallback::operator!=(const MessageCallback &callback) const
{
	return !(*this == callback);
}
