#include <core/component/component-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

class ActivityForwarderOptions: public ActivityMultiplexerPluginOptions {
	public:
		ComponentReference target_multiplexer;

		SERIALIZE_OPTIONS(MEMBER(target_multiplexer))
};


