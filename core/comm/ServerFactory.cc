#include "ServerFactory.h"

namespace ServerFactory {

ServiceServer *create_server(const std::string &endpoint_uri) 
{
	URI u(endpoint_uri);
	
	if (u.get_protocol() == "ipc") {
		
		syslog(LOG_NOTICE, "Server::ServerFactory creating IPC server at %s.", u.get_path().c_str());
		
		return new IPCServer(u.get_path(), 10);
		
	} else if (u.get_protocol() == "tcp") {
		
		syslog(LOG_NOTICE, "Server::ServerFactory creating TCP server at %s:%s.", u.get_host().c_str(), u.get_port().c_str());
		
		return new TCPServer(u.get_host(), u.get_port(), 10);
		
	} else
		std::cerr << "Unknown protocol" << std::endl;
	
	return NULL;
}

	
};
