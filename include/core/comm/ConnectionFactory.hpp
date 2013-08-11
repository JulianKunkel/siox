#ifndef CONNECTION_FACTORY_H
#define CONNECTION_FACTORY_H

#include <exception>
#include <string>

#include "ipc/IPCConnection.hpp"
#include "tcp/TCPConnection.hpp"
#include "URI.hpp"

namespace asio = boost::asio;


class ConnectionFactoryException 
   : public std::exception {
	   
public:
	ConnectionFactoryException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
	
};


namespace ConnectionFactory {
	
/**
 * Creates a connection of the appropriate type according to the protocol 
 * especified in the given URI.
 */
Connection *create_connection(Service &service, 
			      asio::io_service &io_service, 
			      const std::string &endpoint_uri);


};

#endif
