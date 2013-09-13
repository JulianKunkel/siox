#ifndef CORE_COMM_MODULE_HPP
#define CORE_COMM_MODULE_HPP

#include <string>
#include <exception>

#include <core/component/Component.hpp>

#include <core/comm/ServiceClient.hpp>
#include <core/comm/ServiceServer.hpp>


using namespace std;

namespace core{
	class CommunicationModule : public Component {
	protected:		
		virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException) = 0;
		virtual ServiceClient * startClientService(const string & server_address) throw(CommunicationModuleException) = 0;		
	public:
		
		//virtual void setWorkProcessor() = 0; 
		ServiceServer * startServerService(const string & address, ServerCallback * msg_rcvd_callback) throw(CommunicationModuleException){
			assert(msg_rcvd_callback);
			ServiceServer * server = startServerService(address);
			server->setMessageCallback(msg_rcvd_callback);
			try{
				server->listen();
			}catch(CommunicationModuleException & e){
				delete(server);
				throw e;
			}
			
			return server;
		}

		ServiceClient * startClientService(const string & server_address, ConnectionCallback * ccb, MessageCallback * messageCallback) throw(CommunicationModuleException){
			ServiceClient * client = startClientService(server_address);

			client->setConnectionCallback(ccb);
			client->setMessageCallback(messageCallback);

			client->ireconnect();
			return client;
		}
	};
}

#define CORE_COMM_INTERFACE "core_comm"

#endif