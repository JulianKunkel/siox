#include "ConnectionFactory.h"

namespace ConnectionFactory {

Connection *create_connection(ServiceServer &server, 
			      asio::io_service &ios, 
			      const std::string &endpoint_uri) 
{
	try {
		URI uri(endpoint_uri);
		
		if (uri.protocol() == "ipc")
			return new IPCConnection(server, ios, uri.path());
		else if (uri.protocol() == "tcp")
			return new TCPConnection(server, ios, uri.host(), 
						 uri.port());
			
	} catch(MalformedURIException &muri_e) {
		throw(new ConnectionFactoryException(muri_e.what()));
	} catch(IPCConnectionException &ipc_e) {
		throw(new ConnectionFactoryException(ipc_e.what()));
	} catch(TCPConnectionException &tcp_e) {
		throw(new ConnectionFactoryException(tcp_e.what()));
	}
	
	throw(new ConnectionFactoryException("Unknown error creating connection."));
}


Connection *create_connection(asio::io_service &ios, 
			      const std::string &endpoint_uri) 
{
	try {

		URI uri(endpoint_uri);
		
		if (uri.protocol() == "ipc")
			return new IPCConnection(ios, uri.path());
		else if (uri.protocol() == "tcp") 
			return new TCPConnection(ios, uri.host(), uri.port());

		
	} catch(MalformedURIException &muri_e) {
		throw(new ConnectionFactoryException(muri_e.what()));
	} catch(IPCConnectionException &ipc_e) {
		throw(new ConnectionFactoryException(ipc_e.what()));
	} catch(TCPConnectionException &tcp_e) {
		throw(new ConnectionFactoryException(tcp_e.what()));
	}

	throw(new ConnectionFactoryException("Unknown error creating connection."));
}

}