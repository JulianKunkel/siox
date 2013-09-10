#include <core/comm/ServiceClient.hpp>

ServiceClient::ServiceClient( const std::string & endpoint_uri,
                              std::size_t worker_pool_size )
	: io_service_(),
	  worker_pool_size_( worker_pool_size )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Creating service client at %s.",
	             endpoint_uri.c_str() );
#endif
	try {
		connection_ =
		    ConnectionFactory::create_connection( *this, io_service_,
		            endpoint_uri );
	} catch( ConnectionFactoryException & e ) {
		throw( new ServiceClientException( e.what() ) );
	}

}

void ServiceClient::run()
{
	for( std::size_t i = 0; i < worker_pool_size_; ++i ) {
		boost::shared_ptr<boost::thread> worker(
		    new boost::thread( boost::bind(
		                           &asio::io_service::run, &io_service_ ) ) );
		workers.push_back( worker );
	}
}


void ServiceClient::stop()
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Waiting for client workers to finish." );
#endif
	io_service_.stop();

	for( std::size_t i = 0; i < workers.size(); ++i )
		workers[i]->join();

}


void ServiceClient::register_error_callback( Callback & err_cb )
{
	error_callbacks.push_back( &err_cb );
}


void ServiceClient::clear_error_callbacks()
{
	error_callbacks.clear();
}


void ServiceClient::register_response_callback( Callback & rsp_cb )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Registering response callback" );
#endif
	response_callbacks.push_back( &rsp_cb );
}


void ServiceClient::clear_response_callbacks()
{
	response_callbacks.clear();
}


void ServiceClient::isend( boost::shared_ptr<ConnectionMessage> msg )
{
	connection_->isend( msg );
}


void ServiceClient::handle_message( ConnectionMessage & msg,
                                    Connection & connnection )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Handling message response." );
#endif
	boost::ptr_list<Callback>::iterator i;
	for( i = response_callbacks.begin(); i != response_callbacks.end(); ++i ) {
#ifndef NDEBUG
		logger->log( Logger::DEBUG, "Executing message response callback." );
#endif
		i->execute( msg );
	}
}


void ServiceClient::handle_message( boost::shared_ptr<ConnectionMessage> msg,
                                    Connection & connection )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Handling message response." );
#endif
	boost::ptr_list<Callback>::iterator i;
	for( i = response_callbacks.begin(); i != response_callbacks.end(); ++i ) {
#ifndef NDEBUG
		logger->log( Logger::DEBUG, "Executing message response callback." );
#endif
		i->execute( msg );
	}
}
