#include "../ipc/IPCServer.hpp"

IPCServer::IPCServer( const std::string & path,
                      const std::size_t worker_pool_size )
	: ServiceServer( worker_pool_size ),
	  new_connection_(),
	  acceptor_( io_service_ )
{
	unlink( path.c_str() );

	try {
		asio::local::stream_protocol::endpoint endpoint( path );

		acceptor_.open( endpoint.protocol() );
		acceptor_.bind( endpoint );
		acceptor_.listen();

	} catch( boost::system::system_error & e ) {
		throw( new IPCServerException( "Unable to create IPC server." ) );
	}

	try {
		start_accept();
	} catch( IPCServerException & ipc_e ) {
		throw;
	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "IPC server created and accepting connections." );
#endif
}


void IPCServer::start_accept()
{
	try {
		new_connection_.reset( new IPCConnection( *this, io_service_ ) );
	} catch( IPCConnectionException ipc_e ) {
		throw( new IPCServerException( "Unable to create IPC connection." ) );
	}

	acceptor_.async_accept( new_connection_->socket(),
	                        boost::bind( &IPCServer::handle_accept, this,
	                                     asio::placeholders::error ) );
}


void IPCServer::handle_accept( const boost::system::error_code & err )
{
	if( !err ) {

#ifndef NDEBUG
		logger->log( Logger::DEBUG, "New IPC connection accepted." );
#endif
		new_connection_->start();

		IPCConnection_ptr connection( new_connection_ );
		connections_.push_back( connection );
	}

	try {
		start_accept();
	} catch( IPCServerException & ipc_e ) {
		throw;
	}

}
