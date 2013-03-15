#include "QueuedMessageConnection.h"


QueuedMessageConnection::QueuedMessageConnection(const std::string address) 
{
	remote_address = new URI(address);
}


QueuedMessageConnection::QueuedMessageConnection() 
{
}


QueuedMessageConnection::~QueuedMessageConnection() 
{ 
	
}


void QueuedMessageConnection::disconnect()
{
}


void QueuedMessageConnection::reconnect()
{
}


void QueuedMessageConnection::send(Message &message)
{
}


void QueuedMessageConnection::queued_send(Message &message)
{
}


Message *QueuedMessageConnection::recv()
{
	
}


void QueuedMessageConnection::add_message_callback(MessageCallback &callback, 
						   int direction)
{

}


void QueuedMessageConnection::remove_message_callback(MessageCallback &callback, 
						int direction)
{

}


bool 
QueuedMessageConnection::operator==(const QueuedMessageConnection &conn) const
{
	if (this == &conn)
		return true;
	
	return false;
}


bool 
QueuedMessageConnection::operator!=(const QueuedMessageConnection &conn) const
{
	return !(*this == conn);
}
