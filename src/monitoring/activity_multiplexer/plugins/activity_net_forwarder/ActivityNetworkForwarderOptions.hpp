#include <string>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ActivityNetworkForwarderClientOptions: public ActivityMultiplexerPluginOptions {
	public:
		// Use communication component with specified target address
		ComponentReference comm;
		string targetAddress;

		// When the forwardAllActivities is set to false, all the Activities are sent to the reasoner by the AN forwarder client
		bool forwardAllActivities;
		ComponentReference reasoner;

		// Ringbuffersize unit is bytes
		uint ringBufferSize = 0;
};



//@serializable
class ActivityNetworkForwarderServerOptions: public ComponentOptions {
	public:
		// Options for target multiplexer empty at the moment
		ComponentReference target_multiplexer;

		// Use communication component with specified target address
		ComponentReference comm;
		string serviceAddress;		
};

