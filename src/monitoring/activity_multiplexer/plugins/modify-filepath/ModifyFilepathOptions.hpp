#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class ModifyFilepathOptions: public ActivityMultiplexerPluginOptions {
	public:
        string interface;
        string implementation;
};


