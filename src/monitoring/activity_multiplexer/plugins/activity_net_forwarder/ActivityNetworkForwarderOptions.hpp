#include <string>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ActivityNetworkForwarderClientOptions: public ActivityMultiplexerPluginOptions {
	public:
		ComponentReference comm;
		string targetAddress;
};



//@serializable
class ActivityNetworkForwarderServerOptions: public ComponentOptions {
	public:
		ComponentReference target_multiplexer;

		ComponentReference comm;
		string serviceAddress;		
};

