#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ActivityMutatorOptions: public ActivityMultiplexerPluginOptions {
	public:
		ComponentReference target_multiplexer;
};


