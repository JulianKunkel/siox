#include "ServerFactory.hpp"

namespace ServerFactory {

ServiceServer *create_server(const std::string &endpoint_uri) 
{
	try {
		URI uri(endpoint_uri);
	
		if (uri.protocol() == "ipc") 
			return new IPCServer(uri.path(), 10);
		else if (uri.protocol() == "tcp")
			return new TCPServer(uri.host(), uri.port(), 10);
	
	} catch(MalformedURIException &muri_e) {
		throw(new ServerFactoryException(muri_e.what()));
	} catch(IPCServerException &ipc_e) {
		throw(new ServerFactoryException(ipc_e.what()));
	} catch(TCPServerException &tcp_e) {
		throw(new ServerFactoryException(tcp_e.what()));
	}
	
	throw(new ServerFactoryException("Unknown error creating server."));

}

	
};
