#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ActivityBinWriterPluginOptions: public ActivityMultiplexerPluginOptions {
	public:
		string filename;
};


