#ifndef SERVICE_H
#define SERVICE_H

#include "ConnectionMessage.h"

class Service {
public:
	virtual void handle_message(ConnectionMessage &msg) = 0;
	virtual void handle_message(boost::shared_ptr<ConnectionMessage> msg) = 0;
};

#endif