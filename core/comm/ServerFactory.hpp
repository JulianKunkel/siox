#ifndef SERVER_FACTORY_H
#define SERVER_FACTORY_H

#include <exception>
#include <string>

#include "ipc/IPCServer.hpp"
#include "tcp/TCPServer.hpp"
#include "misc/URI.hpp"

namespace asio = boost::asio;

class ServerFactoryException 
   : public std::exception {
	  
public:
	ServerFactoryException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};

namespace ServerFactory {
	
/**
 * Creates a connection of the appropriate type according to the protocol 
 * especified in the given URI.
 */
ServiceServer *create_server(const std::string &endpoint_uri);
	
};

#endif
