//#include <knowledge/optimizer/OptimizerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

#include <vector>
#include <utility>

using namespace std;
using namespace monitoring;


//@serializable
struct AnomalyInjectorPluginOptions : public ActivityMultiplexerPluginOptions{

	// ComponentID to attribute anomalies to
	// TODO

	// Random seed for plugin; set to 0 for "true random"
	// TODO

	// Mean of Geometric distribution for interval between two anomaly injections in ms
	float intervalMean = 80.0;

	// Mean and variance of Gaussian distribution for delta_time_ms
	float deltaTimeMean = 0.0;
	float deltaTimeVariance = 10.0;
};

