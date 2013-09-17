#include "../tcp/TCPServer.hpp"

TCPServer::TCPServer( const std::string & address, const std::string & port,
                      const std::size_t worker_pool_size )
	: ServiceServer( worker_pool_size ),
	  acceptor_( io_service_ ),
	  new_connection_()
{
	asio::ip::tcp::resolver resolver( io_service_ );
	asio::ip::tcp::resolver::query query( address, port );

	try {
		asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );

		acceptor_.open( endpoint.protocol() );
		acceptor_.set_option( asio::ip::tcp::acceptor::reuse_address( true ) );
		acceptor_.bind( endpoint );
		acceptor_.listen();

	} catch( boost::system::system_error & e ) {
		throw( new TCPServerException( "Unable to create TCP server." ) );
	}

	try {
		start_accept();
	} catch( TCPServerException & tcp_e ) {
		throw;
	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "TCP server created and accepting connections." );
#endif

}


void TCPServer::start_accept()
{
	try {
		new_connection_.reset( new TCPConnection( *this, io_service_ ) );
	} catch( TCPConnectionException tcp_e ) {
		throw( new TCPServerException( "Unable to create TCP connection." ) );
	}

	acceptor_.async_accept( new_connection_->socket(),
	                        boost::bind( &TCPServer::handle_accept, this,
	                                     asio::placeholders::error ) );
}


void TCPServer::handle_accept( const boost::system::error_code & error )
{
	if( !error ) {

#ifndef NDEBUG
		logger->log( Logger::DEBUG, "New TCP connection accepted." );
#endif
		new_connection_->start();

		TCPConnection_ptr connection( new_connection_ );
		connections_.push_back( connection );
	}

	try {
		start_accept();
	} catch( TCPServerException & tcp_e ) {
		throw;
	}

}

