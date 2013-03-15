#include "ListenerFactory.h"

#include "IPCListener.h"
#include "TCPListener.h"
#include "URI.h"

#include <iostream>

namespace ListenerFactory {

Listener *create_listener(MessageServer &server, 
			  const std::string listen_address)
{
	URI uri(listen_address);

	Listener l;
	
	if (uri.protocol == "ipc")
		return new IPCListener(uri.host);
	else if (uri.protocol == "tcp")
		return new TCPListener();
	else
		std::cout << "Unknown protocol" << std::endl;
}
	
};