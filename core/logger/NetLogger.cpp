#include <core/logger/NetLogger.hpp>


NetLogger::NetLogger( const std::string & server_uri )
{
	try {
		netservice_ = new ServiceClient( server_uri, NETLOGGER_THREADS );
	} catch( ServiceClientException & e ) {
		return;
	}
	netservice_->run();
}


NetLogger::~NetLogger()
{
	netservice_->stop();
}


void NetLogger::log_append( const Priority prio, const char * buffer )
{
	boost::shared_ptr<siox::MessageBuffer> mp( new siox::MessageBuffer() );
	mp->set_action( siox::MessageBuffer::Log );
	mp->set_priority( prio );
	mp->set_logmsg( buffer );

	boost::shared_ptr<ConnectionMessage> msg_ptr( new ConnectionMessage( mp ) );
	netservice_->isend( msg_ptr );
}
