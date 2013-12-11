#include <assert.h>
#include <iostream>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>
#include <core/hcomm/HighLevelCommunicationModule.hpp>

#include <util/JobProcessor.hpp>

#include <monitoring/activity_multiplexer/plugins/activity_net_forwarder/ActivityBinarySerializable.hpp>

#include <util/TestHelper.hpp>

#include "../HighLevelCommunicationModuleOptions.hpp"

using namespace core;
using namespace std;
using namespace util;

#define ADDRESS1 "ipc://gamma#channel1"
#define ADDRESS2 "ipc://gamma#channel2"

void test1(HLCommunicationModule * hcomm){
	HLCommServiceServer * s1 = hcomm->startServerService( ADDRESS1 );
	HLCommServiceServer * s2 = hcomm->startServerService( ADDRESS2 );

	HLCommServiceClient * c1 = hcomm->startClientService( ADDRESS1 );
	HLCommServiceClient * c2 = hcomm->startClientService( ADDRESS2 );

	// Add the message handlers as anonymous classes
	struct : public HLUnidirectionalMessageHandler{
		virtual uint64_t serializeMessageLen(const void * msgObject) {
			return j_serialization::serializeLen( (char*) msgObject );
		}

		virtual void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
			j_serialization::serialize( (char*) msgObject, buffer, pos );
		}
	} csh;

	struct : public HLServerCallbackUnidirectional, public ProtectRaceConditions{
			char * string = nullptr;

			virtual void messageReceivedCB(const char * message_data, uint64_t data_size){
				uint64_t pos = 0;
				j_serialization::deserialize( string, message_data, pos, data_size );
				sthHappens();
			}
	} sh;

	s1->registerMessageCallback( 4711, & sh );

	c1->registerUnidirectionalMessage( 4711, & csh );
	c1->isend( 4711, const_cast<char*>("testPointer") );

	try{
		sh.waitUntilSthHappened();
	}catch(TimeoutException & e){
		assert(false);
	}

	delete(c1);
	delete(c2);

	delete(s1);
	delete(s2);
}


int main(){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	HLCommunicationModule * hcomm = core::module_create_instance<HLCommunicationModule>( "", "siox-core-hcomm", CORE_HCOMM_INTERFACE );
  
	assert(comm != nullptr);
	comm->init();

	// init high level communication

	HighLevelCommunicationModuleOptions & o = (HighLevelCommunicationModuleOptions&) hcomm->getOptions();
	o.parentComm.componentPointer = comm; 
	hcomm->init();

	test1( hcomm );

	delete(hcomm);
	delete(comm);
}