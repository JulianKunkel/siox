#include <monitoring/statistics_collector/StatisticsProviderPluginOptions.hpp>


using namespace std;
using namespace monitoring;

class ProviderSkeletonOptions: public StatisticsProviderPluginOptions{
public:
	// put all Options here, e.g.
	//string filename;
	//int level;
	//SERIALIZE_CONTAINER(MEMBER(filename) MEMBER(level))

	// right now we serialize an empty one
	SERIALIZE_OPTIONS()
};


