#include <assert.h>

#include "rpc-sample-interface.hpp"

#include "rpc-server-module-options.hpp"

#include <core/comm/test/rpc-sample-interfaceBinarySerializable.hpp>

#include <core/comm/CommunicationModule.hpp>


using namespace core;

/* 
 This sample RPC server only takes care for one RPC-call, if one module wants to send multiple it either encodes this in a local message type OR it uses the MultiMessageTypeService.
 */
class RPCSampleServerImplementation : public core::Component, ServerCallback{
	virtual ComponentOptions* AvailableOptions(){
		return new RPCServerModuleOptions();
	}

	virtual void init(){
		RPCServerModuleOptions & o = getOptions<RPCServerModuleOptions>();
		CommunicationModule * comm =  GET_INSTANCE(CommunicationModule, o.comm);
		target =  GET_INSTANCE(SampleInterface, o.rpcTarget);

		assert(comm != nullptr);
		assert(target != nullptr);

		server = comm->startServerService(o.commAddress, this);
	}

	virtual void messageReceivedCB(std::shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
		// we expect immediate responses from this RPC server (makes handling easier here).

		DataForRPC2 data;
		uint64_t pos = 0;
		j_serialization::deserialize(data, message_data, pos, buffer_size);
		DataFromRPC2 response = target->rpc2(data);
		msg->isendResponse(& response);
	}

	virtual uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return j_serialization::serializeLen(* (DataFromRPC2*) responseType);
	}

	virtual void serializeResponseMessage(const ServerClientMessage * msg, const void * responseType, char * buffer, uint64_t & pos){
		j_serialization::serialize(* (DataFromRPC2*) responseType, buffer, pos);
	}

	~RPCSampleServerImplementation(){
		delete(server);
	}

private:
	SampleInterface * target;
	ServiceServer * server;
};


extern "C" {
	void * RPCSAMPLE_SERVER_INSTANCIATOR_NAME()
	{
		return new RPCSampleServerImplementation();
	}
}
