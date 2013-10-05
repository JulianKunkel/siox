#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class FileWriterPluginOptions: public ActivityMultiplexerPluginOptions {
	public:
		string filename;
		bool synchronize = false;
};


