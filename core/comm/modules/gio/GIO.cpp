/*
 This class contains an in-memory communication server which allows to InMemory inter-module communication.
 */
#include <assert.h>
#include <iostream>

#include <map>
#include <utility>
#include <exception> 
#include <thread>

#include <glib.h>
#include <gio/gio.h>

#include <core/comm/CommunicationModuleImplementation.hpp>

#include "GIOOptions.hpp"

using namespace core;
using namespace std;


// This function splits the string <hostname>:<port> into the tupel
static pair<string, uint16_t> splitAddress(const string & address){
	int pos = address.find(":");
	if (pos == -1){
		uint16_t port = (uint16_t) atoi(address.c_str());
		if (port == 0){
			throw CommunicationModuleException("Invalid address " + address);
		}
		return {"localhost", port };
	}

	uint16_t port = (uint16_t) atoi(address.substr(pos+1).c_str());
	if (port == 0){
		throw CommunicationModuleException("Invalid address " + address);
	}
	
	return {address.substr(0, pos), port};
}

#include "GIOClient.cpp"
#include "GIOServer.cpp"


class GIOCommModule : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException){
		pair<string, uint16_t> out = splitAddress(address);

		return new GIOServiceServer(out.first, out.second);
	}

	virtual ServiceClient * startClientService(const string & server_address, ConnectionCallback & ccb) throw(CommunicationModuleException){

		auto sc = new GIOClient(server_address);
		sc->setConnectionCallback(& ccb);
		sc->ireconnect();
		return sc;
	}

	virtual void init(){
		g_type_init();
	}

	virtual ComponentOptions* AvailableOptions(){
		return new GIOOptions();
	}
};

extern "C" {
	void * CORE_COMM_INSTANCIATOR_NAME()
	{
		return new GIOCommModule();
	}
}
