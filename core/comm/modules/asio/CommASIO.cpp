#include <core/comm/CommunicationModuleImplementation.hpp>

#include "CommASIO.hpp"

#include "ServerFactory.hpp"
#include "ConnectionFactory.hpp"
#include "ServiceClient.hpp"


core::ServiceServer * CommASIO::startServerService(const string & address) throw(core::CommunicationModuleException){
	ServiceServer * server_ = ServerFactory::create_server( address );
	server_->run();

	return nullptr;
}

core::ServiceClient * CommASIO::startClientService(const string & server_address, core::ConnectionCallback & ccb) throw(core::CommunicationModuleException){

	ServiceClient * client;
	try {
		client = new ServiceClient( server_address, 1 );
	} catch( ServiceClientException & e ) {
		throw core::CommunicationModuleException(e.what());
	}
	client->run();

	return nullptr;
}



extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new CommASIO();
	}
}
