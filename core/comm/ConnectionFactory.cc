#include "ConnectionFactory.h"

namespace ConnectionFactory {

Connection *create_connection(ServiceServer &server, 
			      asio::io_service &ios, 
			      const std::string &endpoint_uri) 
{
	URI u(endpoint_uri);
	
	if (u.get_protocol() == "ipc")
		return new IPCConnection(server, ios, u.get_path());
	else if (u.get_protocol() == "tcp")
		return new TCPConnection(server, ios, u.get_host(), 
					 u.get_port());
	else
		std::cerr << "Unknown protocol" << std::endl;
	
	return NULL;
}


Connection *create_connection(asio::io_service &ios, 
			      const std::string &endpoint_uri) 
{
	URI u(endpoint_uri);
	
	if (u.get_protocol() == "ipc")
		return new IPCConnection(ios, u.get_path());
	else if (u.get_protocol() == "tcp") {
		
		syslog(LOG_NOTICE, "Client::Factory creating TCP connection for io_service at %s.", endpoint_uri.c_str());
		return new TCPConnection(ios, u.get_host(), u.get_port());
	} else
		std::cerr << "Unknown protocol" << std::endl;
	
	return NULL;
}

}