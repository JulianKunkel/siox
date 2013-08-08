#ifndef NET_LOGGER_H
#define NET_LOGGER_H

#define NETLOGGER_THREADS 1

#include <string>

#include "Logger.hpp"
#include "ServiceClient.hpp"

class NetLogger 
   : public Logger {

public:
	NetLogger(const std::string &server_uri);
	~NetLogger();
	
protected:
	void log_append(const Priority prio, const char *buffer);

private:
	ServiceClient *netservice_;
};

#endif