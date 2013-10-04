#include <assert.h>
#include <iostream>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>

#include "rpc-sample-interface.hpp"

using namespace core;

/* This class represents the final RPC-target, thus could be any existing module*/
class RPCTargetImplementation : public SampleInterface{
	void rpc1(const DataForRPC1 & data){
		cout << "rpc1" << endl;
	}

	DataFromRPC2 rpc2(const DataForRPC2 & data){
		cout << "rpc2 " << data.b << endl;
		DataFromRPC2 x = {1, "testResponse"};
		return x;
	}

	virtual ComponentOptions* AvailableOptions(){
		// whatever options the module has...
		return new ComponentOptions();
	}

	virtual void init(){

	}

};

int main(){
	// the creation of comm etc. is usually done by the auto-configurator as specified in the configuration file, but we do it manually here for the purpose of the test.

	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	SampleInterface * client = core::module_create_instance<SampleInterface>( "", "siox-core-comm-rpc-client", RPCSAMPLE_CLIENT_INTERFACE );
	Component * server = core::module_create_instance<Component>( "", "siox-core-comm-rpc-server", RPCSAMPLE_SERVER_INTERFACE );

	server->init();
	client->init();

	// run the real test using RPC:
	client->rpc1({2});
	client->rpc1({5});
	DataFromRPC2 rpc2_ret = client->rpc2({"testString"});
	cout << rpc2_ret.c << " " << rpc2_ret.d << endl;

	delete(client);
	delete(server);
	delete(comm);
}