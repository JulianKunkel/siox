#include <assert.h>
#include <iostream>

#include <core/module/ModuleLoader.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/comm/NetworkService.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <knowledge/reasoner/AnomalyTrigger.hpp>

#include <util/TestHelper.hpp>

#include "../ActivityNetworkForwarderClient.hpp"
#include "../ActivityNetworkForwarderOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;
using namespace knowledge;



class MyTargetMuxStub : public ActivityMultiplexer, public ProtectRaceConditions{
public:
	int receivedActivities = 0;

	shared_ptr<Activity> logged_activity = nullptr;

	void Log( const shared_ptr<Activity> & activity ) override{
		cout << "Logging: " << activity->ucaid_ << endl;

		logged_activity = activity;
		receivedActivities++;
		sthHappens();
	}

	void registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, Callback handler, bool async ) override {}
	void unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, bool async ) override {}
	void registerCatchall( ActivityMultiplexerListener* listener, Callback handler, bool async ) override {}
	void unregisterCatchall( ActivityMultiplexerListener* listener, bool async ) override {}

	core::ComponentOptions* AvailableOptions() override{
		return nullptr;
	}

	void init() override{

	}
};

// test for the ringbuffer.
void test2(){
	MyTargetMuxStub mySourceMux;
	MyTargetMuxStub myTargetMux;

	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	NetworkService * server = core::module_create_instance<NetworkService>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", NETWORK_SERVICE_INTERFACE );
	ActivityNetworkForwarderClient * client = core::module_create_instance<ActivityNetworkForwarderClient>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	// now wire the tests together.
	ActivityNetworkForwarderServerOptions * so = new ActivityNetworkForwarderServerOptions();
	so->target_multiplexer.componentPointer = & myTargetMux;
	so->comm.componentPointer = comm;
	so->serviceAddress = "localhost:3032";
	server->init(so);

	ActivityNetworkForwarderClientOptions * co = new ActivityNetworkForwarderClientOptions();
	co->multiplexer.componentPointer = & mySourceMux;
	co->comm.componentPointer = comm;
	co->targetAddress = "localhost:3032";
	co->forwardAllActivities = false;
	co->ringBufferSize = 4;
	((Component*) client)->init(co);

	shared_ptr<Activity> activity (new Activity());
	activity->ucaid_ = 4711;

	shared_ptr<Activity> activity2 (new Activity());
	activity2->ucaid_ = 4712;

	client->NotifyAsync( activity, 0 );
	client->NotifyAsync( activity, 0 );
	client->NotifyAsync( activity, 0 );
	client->NotifyAsync( activity, 0 );
	client->NotifyAsync( activity, 0 );
	client->NotifyAsync( activity2, 0 );

	assert(! myTargetMux.hasSthHappened());

	AnomalyTrigger * atrigger = dynamic_cast<AnomalyTrigger*>(client);
	assert( atrigger );
	atrigger->triggerResponseForAnomaly(false);

	int count = 0;
	while(myTargetMux.receivedActivities != 4){
		myTargetMux.waitUntilSthHappened();
		count ++;
		assert (count < 5);
	}
	assert( myTargetMux.logged_activity->ucaid_ == activity2->ucaid_ );
	
	client->NotifyAsync( activity, 0 );
	atrigger->triggerResponseForAnomaly(false);

	myTargetMux.waitUntilSthHappened();

	assert( myTargetMux.receivedActivities == 5 );

	assert( myTargetMux.logged_activity->ucaid_ == activity->ucaid_ );

	delete(client);

	delete(server);
	delete(comm);
}

void test(){
		CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();


	MyTargetMuxStub myTargetMux;
	MyTargetMuxStub mySourceMux;

	NetworkService * server = core::module_create_instance<NetworkService>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", NETWORK_SERVICE_INTERFACE );
	ActivityNetworkForwarderClient * client = core::module_create_instance<ActivityNetworkForwarderClient>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	// now wire the tests together.
	ActivityNetworkForwarderServerOptions * so = new ActivityNetworkForwarderServerOptions();
	so->target_multiplexer.componentPointer = & myTargetMux;
	so->comm.componentPointer = comm;
	so->serviceAddress = "localhost:3032";
	server->init(so);

	ActivityNetworkForwarderClientOptions * co = new ActivityNetworkForwarderClientOptions();
	co->multiplexer.componentPointer = & mySourceMux;
	co->comm.componentPointer = comm;
	co->forwardAllActivities = true;
	co->targetAddress = "localhost:3032";
	((Component*) client)->init(co);


	// begin the test
	shared_ptr<Activity> activity (new Activity());
	activity->ucaid_ = 4711;

	client->NotifyAsync( activity, 0 );

	myTargetMux.waitUntilSthHappened();

	assert( myTargetMux.logged_activity->ucaid_ == 4711 );
	cout << "Received activity with UCAID: " << myTargetMux.logged_activity->ucaid_ << endl;

	delete(client);


	delete(server);
	delete(comm);
}

int main(){

	test();
	test2();

	printf("OK\n");

	return 0;
}
