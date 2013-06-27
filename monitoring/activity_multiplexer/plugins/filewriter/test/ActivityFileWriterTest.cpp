#include <core/module/module-loader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

#include "../ActivityFileWriterOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	ActivityMultiplexer * m1 = core::module_create_instance<ActivityMultiplexer>("", "ActivityMultiplexer_Impl1", "monitoring_activitymultiplexer");

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	ActivityMultiplexerPlugin * ap = core::module_create_instance<ActivityMultiplexerPlugin>("", "ActivityFileWriter", "monitoring_activitymultiplexer_plugin");

	// init plugin
	// not necessary, but for testing...
	FileWriterPluginOptions * op = (FileWriterPluginOptions*) ap->get_options();
	op->multiplexer = m1;
	op->filename = "test.txt";

	ap->init(op);

	ComponentID cid = {.pid = {2,3,4}, .uuid= {1,2}};

	auto * parentArray = new vector<ComponentID>{{.pid = {1,2,3}, .uuid= {2,2}}};
	auto * attributeArray = new vector<Attribute>{{.id=111, .value = "myData"}, {.id=3, . value = (uint64_t) 4711}};
	auto * remoteCallsArray = new vector<RemoteCall>();

	// Cast the real activity to the serializable object class wrapper
	Activity * activity = new Activity("test", 3, 5, cid, parentArray, attributeArray, remoteCallsArray, NULL, 0);

	m1->Log(activity);

	m1->shutdown();
}


