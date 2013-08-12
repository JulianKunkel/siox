#include <core/component/component-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

class AnomalySkeletonOptions: public ActivityMultiplexerPluginOptions{
public:
	// put all Options here, e.g.
	//string filename;
	//int level;
	//float latency_in_s;
	//float extreme_high_factor 2
	//float throughput_in_s;
	//SERIALIZE_CONTAINER(MEMBER(filename) MEMBER(level))

	// right now we serialize an empty one
	SERIALIZE_OPTIONS()
};


