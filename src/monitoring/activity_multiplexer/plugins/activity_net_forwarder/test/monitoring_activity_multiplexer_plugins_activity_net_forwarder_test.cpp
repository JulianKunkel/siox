#include <assert.h>
#include <iostream>

#include <core/module/ModuleLoader.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/comm/NetworkService.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

#include <util/TestHelper.hpp>

#include "../ActivityNetworkForwarderOptions.hpp"

using namespace core;
using namespace std;
using namespace monitoring;


class MyTargetMuxStub : public ActivityMultiplexer, public ProtectRaceConditions{
public:
		shared_ptr<Activity> logged_activity = nullptr;

		virtual void Log( shared_ptr<Activity> activity ){
			logged_activity = activity;
			sthHappens();
		}

		virtual void registerListener( ActivityMultiplexerListener * listener ){}


		virtual void unregisterListener( ActivityMultiplexerListener * listener ){}

		virtual core::ComponentOptions* AvailableOptions(){
			return nullptr;
		}

		virtual void init(){

		}
};

int main(){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();


	MyTargetMuxStub myTargetMux;
	MyTargetMuxStub mySourceMux;

	NetworkService * server = core::module_create_instance<NetworkService>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", NETWORK_SERVICE_INTERFACE );
	ActivityMultiplexerPlugin * client = core::module_create_instance<ActivityMultiplexerPlugin>( "", "siox-monitoring-activityPlugin-ActivityNetworkForwarder", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE );

	// now wire the tests together.
	ActivityNetworkForwarderServerOptions * so = new ActivityNetworkForwarderServerOptions();
	so->target_multiplexer.componentPointer = & myTargetMux;
	so->comm.componentPointer = comm;
	so->serviceAddress = "localhost:3031";
	server->init(so);

	ActivityNetworkForwarderClientOptions * co = new ActivityNetworkForwarderClientOptions();
	co->multiplexer.componentPointer = & mySourceMux;
	co->comm.componentPointer = comm;
	co->targetAddress = "localhost:3031";
	((Component*) client)->init(co);


	// begin the test
	shared_ptr<Activity> activity (new Activity());
	activity->ucaid_ = 4711;

	client->NotifyAsync(0, activity);


	myTargetMux.waitUntilSthHappened();

	assert( myTargetMux.logged_activity->ucaid_ == 4711 );
	cout << "Received activity with UCAID: " << myTargetMux.logged_activity->ucaid_ << endl;

	delete(server);
	delete(client);
	delete(comm);
	return 0;
}
