#include <core/comm/ServiceServer.hpp>


ServiceServer::ServiceServer( const std::size_t worker_pool_size )
	: worker_pool_size_( worker_pool_size )
{
}


void ServiceServer::run()
{
	for( std::size_t i = 0; i < worker_pool_size_; ++i ) {
		boost::shared_ptr<boost::thread> worker(
		    new boost::thread( boost::bind(
		                           &asio::io_service::run, &io_service_ ) ) );
		workers_.push_back( worker );
	}
}


void ServiceServer::stop()
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Waiting for server workers to finish." );
#endif

	handle_stop();
}


void ServiceServer::handle_stop()
{
	io_service_.stop();

	for( std::size_t i = 0; i < workers_.size(); ++i )
		workers_[i]->join();
}


void ServiceServer::advertise( boost::uint64_t mtype )
{
	boost::shared_ptr<buffers::MessageBuffer> mbuf( new buffers::MessageBuffer() );
	mbuf->set_action( buffers::MessageBuffer::Advertise );
	mbuf->set_type( mtype );

	boost::shared_ptr<ConnectionMessage> msg( new ConnectionMessage( mbuf ) );

	std::multimap<boost::uint32_t, Connection *>::iterator i;
	for( i = subscribed_connections.begin(); i != subscribed_connections.end(); ++i ) {
		( *i ).second->isend( msg );
	}

}


void ServiceServer::ipublish( boost::shared_ptr<ConnectionMessage> msg )
{
	if( subscribed_connections.empty() )
		return;
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Publishing on %zu connections.",
	             subscribed_connections.size() );
#endif
	std::pair < std::multimap<boost::uint32_t, Connection *>::iterator,
	    std::multimap<boost::uint32_t, Connection *>::iterator > range
	    = subscribed_connections.equal_range( msg->get_msg()->type() );

	std::multimap<boost::uint32_t, Connection *>::iterator i;
	for( i = range.first; i != range.second; ++i ) {
		( *i ).second->isend( msg );
	}
}


void ServiceServer::register_message_callback( Callback & msg_rcvd_callback )
{
	message_callbacks_.push_back( &msg_rcvd_callback );
}


void ServiceServer::clear_message_callbacks()
{
	message_callbacks_.clear();
}


void ServiceServer::register_error_callback( Callback & err_callback )
{
	error_callbacks_.push_back( &err_callback );
}


void ServiceServer::clear_error_callbacks()
{
	error_callbacks_.clear();
}


void ServiceServer::subscribe( boost::uint32_t mtype, Connection & connection )
{
	subscribed_connections.insert( std::make_pair( mtype, &connection ) );
}


void ServiceServer::handle_message( ConnectionMessage & msg, Connection & connection )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Handling message..." );
#endif
	if( msg.get_msg()->action() == buffers::MessageBuffer::Subscribe ) {
		subscribe( msg.get_msg()->type(), connection );
	}

	boost::ptr_list<Callback>::iterator i;
	for( i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i ) {
#ifndef NDEBUG
		logger->log( Logger::DEBUG, "Executing message response callback." );
#endif
		i->execute( msg );
	}
}


void ServiceServer::handle_message( boost::shared_ptr<ConnectionMessage> msg,
                                    Connection & connection )
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Handling message..." );
#endif
	if( msg->get_msg()->action() == buffers::MessageBuffer::Subscribe ) {
		subscribe( msg->get_msg()->type(), connection );
	}

	boost::ptr_list<Callback>::iterator i;
	for( i = message_callbacks_.begin(); i != message_callbacks_.end(); ++i ) {
#ifndef NDEBUG
		logger->log( Logger::DEBUG, "Executing message response callback." );
#endif
		i->execute( msg );
	}
}
