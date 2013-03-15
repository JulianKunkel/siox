#include "MessageServer.h"

MessageServer::MessageServer()
{
}


MessageServer::~MessageServer() 
{ 
}


void MessageServer::add_listener(Listener &listener)
{
	listeners.push_back(listener);
}


void MessageServer::remove_listener(Listener &listener)
{
	listeners.remove(listener);
}


void MessageServer::poll_listeners()
{
}
