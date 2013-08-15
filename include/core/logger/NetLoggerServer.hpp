#ifndef NET_LOGGER_SERVER_H
#define NET_LOGGER_SERVER_H

#include <string>

#include <core/logger/Logger.hpp>
#include <core/logger/SioxLogger.hpp>
#include <core/comm/ServerFactory.hpp>

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

