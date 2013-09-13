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

#define MAGIC_KEY 1337

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

static inline uint32_t clientMsgHeaderLen(){
	const uint32_t magicKey = 0;
	const uint32_t clientSidedID = 0;
	const uint64_t msgLength = 0;
	return j_serialization::serializeLen(magicKey) + j_serialization::serializeLen(clientSidedID) + j_serialization::serializeLen(msgLength);
}

static void serializeMsgHeader(char * buffer, uint64_t & pos, uint64_t size, uint32_t magicKey, uint32_t clientSidedID)
{
	j_serialization::serialize(magicKey, buffer, pos);
	j_serialization::serialize(size, buffer, pos);
	j_serialization::serialize(clientSidedID, buffer, pos);
}

static void deserializeMsgHeader(char * buffer, uint64_t & pos, uint64_t msgLength, uint64_t & size, uint32_t & magicKey, uint32_t & clientSidedID)
{
	j_serialization::deserialize(magicKey, buffer, pos, msgLength);
	j_serialization::deserialize(size, buffer, pos, msgLength);
	j_serialization::deserialize(clientSidedID, buffer, pos, msgLength);
}



#include "GIOClient.cpp"
#include "GIOServer.cpp"


class GIOCommModule : public CommunicationModule {
public:
	//virtual void setWorkProcessor() = 0; 
	virtual ServiceServer * startServerService(const string & address) throw(CommunicationModuleException){		
		return new GIOServiceServer(address);
	}

	virtual ServiceClient * startClientService(const string & server_address) throw(CommunicationModuleException){
		return new GIOClient(server_address);
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
