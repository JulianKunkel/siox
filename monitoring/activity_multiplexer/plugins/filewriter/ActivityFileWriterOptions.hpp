#include <core/container/container-serializer.hpp>
#include <core/container/container-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

class FileWriterPluginOptions: public ActivityMultiplexerPluginOptions{
public:
	string filename;

	SERIALIZE(MEMBER(filename))
};


