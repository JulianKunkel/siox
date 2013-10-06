/*
 This class contains an in-memory communication server which allows to InMemory inter-module communication.
 */

#include <core/comm/CommunicationModuleImplementation.hpp>

#include "GIOOptions.hpp"
#include "GIOClient.hpp"
#include "GIOServer.hpp"

using namespace core;
using namespace std;


class GIOCommModule : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address, ProcessorQueue * sendQueue) throw(CommunicationModuleException){		
		return new GIOServiceServer(address, sendQueue);
	}

	virtual ServiceClient * startClientService(const string & server_address, ProcessorQueue * sendQueue) throw(CommunicationModuleException){
		return new GIOClient(server_address, sendQueue);
	}

	virtual void init(){
		setenv("GIO_USE_VFS", "local", 0 );		
		setenv("GVFS_DISABLE_FUSE", "true" , 0);
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
