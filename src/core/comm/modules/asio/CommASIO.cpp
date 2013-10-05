#include <map>

#include <core/comm/CommunicationModuleImplementation.hpp>
#include "CommASIO.hpp"

#include "ServerFactory.hpp"
#include "ConnectionFactory.hpp"
#include "ServiceClient.hpp"
#include "ServiceServer.hpp"
#include "Callback.hpp"

// Combatibility layer

//core::ServiceClient


class ASIOServiceServer : public core::ServiceServer, public Callback{
private:
	std::map<uint32_t, core::ServerCallback*> callbacks;
public:
	::ServiceServer * server;

	ASIOServiceServer(const string & address){
		try{
			server = ServerFactory::create_server( address );
			server->register_message_callback(*this);
			server->run();
		}catch(exception & e){
			throw core::CommunicationModuleException(e.what());
		}
	}


	void execute( ConnectionMessage & msg ) const {
		uint32_t type = msg.get_msg()->type();
		core::ServerCallback* cb = callbacks[type];
		if(cb != nullptr){
			
		}
	}

	void execute( boost::shared_ptr<ConnectionMessage> msg ) const {
	}	

	~ASIOServiceServer(){
		delete(server);
	}

	/** Advertises a new message type among all connected clients. */
	void advertise(uint32_t mtype ){
		assert(false);
	}

	/** Sends the message to all clients who subscribed the message's type */
	void ipublish( std::shared_ptr<core::CreatedMessage> msg ){
		assert(false);
	}


	void register_message_callback(uint32_t mtype, core::ServerCallback * msg_rcvd_callback){
		callbacks[mtype] = msg_rcvd_callback;
	}

	void unregister_message_callback(uint32_t mtype){
		callbacks.erase(mtype);
	}

};

core::ServiceServer * CommASIO::startServerService(const string & address) throw(core::CommunicationModuleException){
	return new ASIOServiceServer(address);
}

core::ServiceClient * CommASIO::startClientService(const string & server_address, core::ConnectionCallback & ccb) throw(core::CommunicationModuleException){

	ServiceClient * client;
	try {
		client = new ServiceClient( server_address, 1 );
	} catch( ServiceClientException & e ) {
		throw core::CommunicationModuleException(e.what());
	}
	client->run();
	//client->register_error_callback( Callback & err_cb );
	//client->register_response_callback( Callback & rsp_cb );


	return nullptr;
}



extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new CommASIO();
	}
}
