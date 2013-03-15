#include "IPCConnection.h"

IPCConnection::IPCConnection(const std::string address) 
	: QueuedMessageConnection(address)
{
}


IPCConnection::IPCConnection(Listener &listener) 
{
}


IPCConnection::~IPCConnection() 
{ 
	
}


void IPCConnection::disconnect()
{
}


void IPCConnection::reconnect()
{
}


void IPCConnection::send()
{
}
