#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ActivityBDBWriterPluginOptions: public ActivityMultiplexerPluginOptions {
	public:
		string dirname;
		bool synchronize = false;
		bool durable = false;
		uint32_t cacheSizeMByte = 10;
};


