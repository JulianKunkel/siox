#include <core/module/module-loader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>


#include "../AnomalySkeletonOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	ActivityMultiplexer * m1 = core::module_create_instance<ActivityMultiplexer>("", "ActivityMultiplexer_Impl1", "monitoring_activitymultiplexer");

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	ActivityMultiplexerPlugin * ap = core::module_create_instance<ActivityMultiplexerPlugin>("", "AnomalySkeleton", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE);

	AnomalySkeletonOptions * op = (AnomalySkeletonOptions*) ap->get_options();
	ActivityPluginDereferencing * facade = core::module_create_instance<ActivityPluginDereferencing>("", "ActivityPluginDereferencingStandardImpl", ACTIVITY_DEREFERENCING_FACADE_INTERFACE);

	ap->init(op, m1, facade);




	ComponentID cid = {.pid = {2,3,4}, .num=1};

	auto * parentArray = new vector<ActivityID>{{{.pid = {1,2,3}, .num=2}, .num = 1} };
	auto * attributeArray = new vector<Attribute>{{.id=111, .value = "myData"}, {.id=3, . value = (uint64_t) 4711}};
	auto * remoteCallsArray = new vector<RemoteCall>();


	UniqueComponentActivityID aid = 4;
	// Cast the real activity to the serializable object class wrapper
	Activity * activity = new Activity(aid, 3, 5, cid, parentArray, attributeArray, remoteCallsArray, nullptr, 0);

	m1->Log(activity);

	m1->shutdown();

	cout << "OK" << endl;
}


