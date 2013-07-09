#include <core/component/component-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

class AnomalySkeletonOptions: public ActivityMultiplexerPluginOptions{
public:
	// put all Options here, e.g.
	//string filename;
	//int level;
	//SERIALIZE_CONTAINER(MEMBER(filename) MEMBER(level))

	// right now we serialize an empty one
	SERIALIZE_OPTIONS()
};


