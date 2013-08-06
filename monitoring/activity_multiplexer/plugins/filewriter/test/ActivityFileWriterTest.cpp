#include <core/module/module-loader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

#include "../ActivityFileWriterOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	ActivityMultiplexer * m1 = core::module_create_instance<ActivityMultiplexer>("", "ActivityMultiplexerAsyncThreaded", "monitoring_activitymultiplexer");

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	ActivityMultiplexerPlugin * ap = core::module_create_instance<ActivityMultiplexerPlugin>("", "ActivityFileWriter", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE);

	// init plugin
	// not necessary, but for testing...
	FileWriterPluginOptions & op = (FileWriterPluginOptions &) ap->getOptions();
	op.filename = "test.txt";

	ap->init(m1, nullptr);

	auto parentArray = vector<ActivityID>{  {.cid = {.pid = {2,3,4}, .id=1}, .id = 1} };
	auto attributeArray = vector<Attribute>{{.id=111, .value = VariableDatatype("myData")}, {.id=3, . value = (uint64_t) 4711}};
	auto remoteCallsArray = vector<RemoteCall>();


	UniqueComponentActivityID aid = 4;
	ActivityID aaid = {.cid = {.pid = {2,3,4}, .id=1}, .id = 2};
	// Cast the real activity to the serializable object class wrapper
	Activity * activity = new Activity(aid, 3, 5, aaid, parentArray, attributeArray, remoteCallsArray, NULL, 0);

	m1->Log(activity);

	delete(m1);
	delete(ap);

	cout << "OK" << endl;
}


