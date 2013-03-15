#include "Listener.h"


Listener::Listener(MessageServer &server, const std::string listen_address)
{
}


Listener::Listener()
{
}


Listener::~Listener()
{
}


bool Listener::operator==(const Listener &listener) const
{
	if (this == &listener)
		return true;
	
	return false;
}


bool Listener::operator!=(const Listener &listener) const
{
	return !(*this == listener);
}


std::string Listener::get_address()
{
}

