#include <core/comm/tcp/TCPConnection.hpp>

TCPConnection::TCPConnection( Service & service,
                              asio::io_service & io_service )
	: Connection( io_service, service ), socket_( io_service )
{
}


TCPConnection::TCPConnection( Service & service,
                              asio::io_service & io_service,
                              const std::string & address,
                              const std::string & port )
	: Connection( io_service, service ), socket_address_( address ),
	  socket_port_( port ), socket_( io_service )
{
	try {
		do_connection();
	} catch( TCPConnectionException & e ) {
		throw;
	}
}


void TCPConnection::do_connection()
{
	asio::ip::tcp::resolver resolver( *io_service_ );
	asio::ip::tcp::resolver::query query( socket_address_, socket_port_ );
	asio::ip::tcp::resolver::iterator endpoint;

	try {
		endpoint = resolver.resolve( query );
	} catch( boost::system::system_error & e ) {
		throw( new TCPConnectionException( "Unable to resolve address." ) );
	}

	asio::async_connect( socket_, endpoint,
	                     boost::bind( &TCPConnection::handle_connect, this,
	                                  asio::placeholders::error ) );
}


void TCPConnection::start()
{
	mtx_bufin_.lock();
	buffer_in_.resize( HEADER_SIZE );

	asio::async_read( socket_, asio::buffer( buffer_in_ ),
	                  asio::transfer_exactly( HEADER_SIZE ),
	                  boost::bind( &TCPConnection::handle_read_header, this,
	                               asio::placeholders::error ) );
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Waiting for message." );
#endif
}


void TCPConnection::start_read_body( unsigned msglen )
{
	buffer_in_.resize( HEADER_SIZE + msglen );
	asio::mutable_buffers_1 buf = asio::buffer( &buffer_in_[HEADER_SIZE], msglen );

	asio::async_read( socket_, buf,
	                  boost::bind( &TCPConnection::handle_read_body, this,
	                               asio::placeholders::error ) );
}


void TCPConnection::isend( boost::shared_ptr<ConnectionMessage> msg )
{
	mtx_bufout_.lock();

	if( !msg->pack( buffer_out_ ) ) {
#ifndef NDEBUG
		logger->log( Logger::ERR, "Error packing message." );
#endif
		mtx_bufout_.unlock();
		return;
	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Sending TCP message (%s).\n",
	             show_hex<std::vector<boost::uint8_t> >( buffer_out_ ).c_str() );
#endif
	asio::async_write( socket_, asio::buffer( buffer_out_ ),
	                   boost::bind( &TCPConnection::handle_write,
	                                this, msg, asio::placeholders::error ) );
}


boost::asio::ip::tcp::socket & TCPConnection::socket()
{
	return socket_;
}


void TCPConnection::do_disconnect()
{
	socket_.close();
}



