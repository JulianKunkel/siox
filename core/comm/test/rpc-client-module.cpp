#include "rpc-sample-interface.hpp"

#include "rpc-client-module-options.hpp"

class RPCSampleClientImplementation : public SampleInterface{
	void rpc1(const DataForRPC1 & data){

	}

	DataFromRPC2 rpc2(const DataForRPC2 & data){
		DataFromRPC2 x;
		return x;
	}

	virtual ComponentOptions* AvailableOptions(){
		return new RPCCLientModuleOptions();
	}

	virtual void init(){

	}

};


extern "C" {
	void * RPCSAMPLE_INSTANCIATOR_NAME()
	{
		return new RPCSampleClientImplementation();
	}
}
