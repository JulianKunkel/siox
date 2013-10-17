#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

//@serializable
class PostgreSQLWriterPluginOptions: public ActivityMultiplexerPluginOptions {
public:
	string db_info;

};


