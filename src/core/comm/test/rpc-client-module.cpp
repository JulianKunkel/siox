#include <map>
#include <mutex>
#include <condition_variable>

#include "rpc-sample-interface.hpp"

#include "rpc-client-module-options.hpp"

#include <core/comm/DefaultCallbacks.hpp>

#include <core/comm/test/rpc-sample-interfaceBinarySerializable.hpp>

#include <core/comm/CommunicationModule.hpp>

using namespace core;
using namespace std;

/*
 This class is an example for a blocking RPC service (multiple RPCs can be in-flight)
 */
struct BlockingRPCMessage{
	DataFromRPC2 response;
	mutex m;
	condition_variable cv; 
};

class RPCSampleClientImplementation : public SampleInterface, MessageCallback{
public:

	void rpc1(const DataForRPC1 & data){
		// at the moment we only wrap RPC2 ...
	}

	DataFromRPC2 rpc2(const DataForRPC2 & data){		
		BlockingRPCMessage container;
		{
		unique_lock<mutex> lock(container.m);
		client->isend(& data, & container);
		// wait for the response
		container.cv.wait(lock);
		}

		return container.response;		
	}

	virtual ComponentOptions* AvailableOptions(){
		return new RPCCLientModuleOptions();
	}

	virtual void init(){
		RPCCLientModuleOptions & o = getOptions<RPCCLientModuleOptions>();
		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, o.comm);
		client = comm->startClientService(o.commAddress, & connCallback, this);
	}

	RPCSampleClientImplementation(){
		// reconnect forever...
		connCallback = DefaultReConnectionCallback();
	}

	~RPCSampleClientImplementation(){
		delete(client);
	}


	virtual void messageSendCB(BareMessage * msg){ /* do nothing */ }

	virtual void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		BlockingRPCMessage * container = (BlockingRPCMessage *) msg->user_ptr;
		uint64_t pos = 0;
		j_serialization::deserialize(container->response, buffer, pos, buffer_size);

		unique_lock<mutex> lock(container->m);
		container->cv.notify_one();
	}

	virtual void messageTransferErrorCB(BareMessage * msg, CommunicationError error){
		// Should happen only if the target server is not offering this RPC service!
		// This should be really critical and kill the caller.
		assert(false);
	}

	virtual uint64_t serializeMessageLen(const void * msgObject) {
		return j_serialization::serializeLen(* (DataForRPC2*) msgObject);
	}

	virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		j_serialization::serialize(* (DataForRPC2*) msgObject, buffer, pos);
	}

private:
	ServiceClient * client;
	ConnectionCallback connCallback;
};


extern "C" {
	void * RPCSAMPLE_INSTANCIATOR_NAME()
	{
		return new RPCSampleClientImplementation();
	}
}
