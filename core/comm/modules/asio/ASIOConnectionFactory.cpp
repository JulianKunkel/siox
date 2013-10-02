#include "ConnectionFactory.hpp"

namespace ConnectionFactory {

	Connection * create_connection( Service & service,
	                                asio::io_service & ios,
	                                const std::string & endpoint_uri )
	{
		try {
			URI uri( endpoint_uri );

			if( uri.protocol() == "ipc" )
				return new IPCConnection( service, ios, uri.path() );
			else if( uri.protocol() == "tcp" )
				return new TCPConnection( service, ios, uri.host(),
				                          uri.port() );

		} catch( MalformedURIException & muri_e ) {
			throw( new ConnectionFactoryException( muri_e.what() ) );
		} catch( IPCConnectionException & ipc_e ) {
			throw( new ConnectionFactoryException( ipc_e.what() ) );
		} catch( TCPConnectionException & tcp_e ) {
			throw( new ConnectionFactoryException( tcp_e.what() ) );
		}

		throw( new ConnectionFactoryException( "Unknown error creating connection." ) );
	}

}