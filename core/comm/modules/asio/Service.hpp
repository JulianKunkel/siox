#ifndef SERVICE_H
#define SERVICE_H

#include "Connection.hpp"
#include "ConnectionMessage.hpp"

class Connection;

class Service {

	public:
		virtual void handle_message( ConnectionMessage & msg,
		                             Connection & connection ) = 0;
		virtual void handle_message( boost::shared_ptr<ConnectionMessage> msg,
		                             Connection & connection ) = 0;
};

#endif