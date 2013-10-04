#include "rpc-sample-interface.hpp"

#include "rpc-server-module-options.hpp"

class RPCSampleServerImplementation : public core::Component{
	virtual ComponentOptions* AvailableOptions(){
		return new RPCServerModuleOptions();
	}

	virtual void init(){

	}
};


extern "C" {
	void * RPCSAMPLE_SERVER_INSTANCIATOR_NAME()
	{
		return new RPCSampleServerImplementation();
	}
}
