#ifndef SERVER_FACTORY_H
#define SERVER_FACTORY_H

#include <string>

#include "ipc/IPCServer.h"
#include "tcp/TCPServer.h"
#include "misc/URI.h"

namespace asio = boost::asio;

namespace ServerFactory {
	
/**
 * Creates a connection of the appropriate type according to the protocol 
 * especified in the given URI.
 */	
ServiceServer *create_server(const std::string &endpoint_uri);
	
};

#endif
