#ifndef NET_LOGGER_SERVER_H
#define NET_LOGGER_SERVER_H

#include <string>

#include "Logger.hpp"
#include "ServerFactory.hpp"
#include "SioxLogger.hpp"

Logger *logger;

class LoggerCallback : public Callback {
public:
	void handle_message(ConnectionMessage &msg) const
	{
		logger->log(static_cast<Logger::Priority>(msg.get_msg()->priority()), 
			    msg.get_msg()->logmsg().c_str());
	}
};


class NetLoggerServer 
   : public Logger {

public:
	NetLoggerServer(const std::string &local_uri);
	~NetLoggerServer();
	
private:
	ServiceServer *netserver_;
};

#endif

