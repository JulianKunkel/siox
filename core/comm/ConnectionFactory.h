#ifndef CONNECTION_FACTORY_H
#define CONNECTION_FACTORY_H

#include <string>

#include "ipc/IPCConnection.h"
#include "tcp/TCPConnection.h"
#include "misc/URI.h"

namespace asio = boost::asio;

namespace ConnectionFactory {
	
/**
 * Creates a connection of the appropriate type according to the protocol 
 * especified in the given URI.
 */	
Connection *create_connection(asio::io_service &io_service, 
			      const std::string &endpoint_uri);
	
};

#endif
