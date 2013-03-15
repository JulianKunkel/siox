#include "Messenger.h"

Messenger::Messenger() 
{
}


Messenger::~Messenger() 
{ 
}


QueuedMessageConnection *Messenger::create_connection(URI &uri)
{
}


QueuedMessageConnection *Messenger::accept_connection(Listener &listener)
{
	
}


QueuedMessageConnection *Messenger::close_connection(QueuedMessageConnection 
						     &connection)
{

}


void Messenger::poll_connections() 
{
}
	

void Messenger::add_connection(QueuedMessageConnection &connection)
{
}


void Messenger::remove_connection(QueuedMessageConnection &connection)
{
}


void Messenger::add_message_callback(QueuedMessageConnection &connection,
				     MessageCallback &callback, int direction)
{
}


void Messenger::remove_message_callback(QueuedMessageConnection &connection,
					MessageCallback &callback, 
					int direction)
{
}

