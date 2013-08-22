#include <core/logger/NetLoggerServer.hpp>

NetLoggerServer::NetLoggerServer( const std::string & local_uri )
{
	netserver_ = ServerFactory::create_server( local_uri );
	netserver_->run();

	LoggerCallback logcb;
	netserver_->register_message_callback( logcb );
}


NetLoggerServer::~NetLoggerServer()
{
	netserver_->stop();
}
