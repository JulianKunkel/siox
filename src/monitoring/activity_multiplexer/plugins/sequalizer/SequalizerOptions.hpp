#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class SequalizerOptions: public ActivityMultiplexerPluginOptions {
	public:
		ComponentReference target_multiplexer;
        string interface;
        string implementation;
};


