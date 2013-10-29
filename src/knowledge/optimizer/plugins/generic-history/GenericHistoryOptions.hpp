#include <knowledge/optimizer/OptimizerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;


//@serializable
struct GenericHistoryOptions : public ActivityMultiplexerPluginOptions{
	ComponentReference optimizer;
};

