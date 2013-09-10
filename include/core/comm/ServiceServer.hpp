#ifndef SERVICE_SERVER_H
#define SERVICE_SERVER_H

#include <memory> 

#include <core/comm/Message.hpp>

namespace core{


class ServerCallback{
public:
	virtual void messageReceivedCB(std::shared_ptr<Connection> connection, std::shared_ptr<Message> msg) = 0;
};

class ServiceServer {
	public:
		/** Advertises a new message type among all connected clients. */
		virtual void advertise(uint32_t mtype ) = 0;

		/** Sends the message to all clients who subscribed the message's type */
		virtual void ipublish( std::shared_ptr<CreatedMessage> msg ) = 0;

		virtual void register_message_callback(uint32_t mtype, ServerCallback * msg_rcvd_callback) = 0;

		virtual void unregister_message_callback(uint32_t mtype) = 0;

		virtual ~ServiceServer(){}
};
}

#endif

