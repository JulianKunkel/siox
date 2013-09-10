#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>

#include <string>
#include <memory>

using namespace std;

namespace core{

enum class ConnectionError : uint8_t{
	CONNECTION_LOST,
	SERVER_NOT_ACCESSABLE,
	MESSAGE_TYPE_NOT_AVAILABLE,
	MESSAGE_INCOMPATIBLE
};

class Connection;
class CreatedMessage;

class ConnectionCallback{
public:
	virtual void connectionErrorCB(Connection & connection, ConnectionError error){}
	virtual void connectionSuccessfullCB(Connection & connection){}
};


class Connection {
protected:
	ConnectionCallback * connectionCallback;
public:
		void setConnectionCallback(ConnectionCallback * ccb){
			this->connectionCallback = ccb;
		}

		/**
	  	 * Try to reconnect asynchronously and resend all currently pending messages. 
	  	 */
		virtual void ireconnect() = 0;

		/**
		 * Return the address of the target communication endpoint.
		 */
		virtual const string & getAddress() const = 0 ;

		/**
		 * Asynchronously sends a message to the ServiceServer.
		 *
		 * @note This function may return immediately.
		 */
		virtual void isend( std::shared_ptr<CreatedMessage> msg ) = 0;

		virtual ~Connection(){}
};

}

#endif
