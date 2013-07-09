#include <core/module/module-loader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <core/autoconfigurator/AutoConfigurator.hpp>


#include "../AnomalySkeletonOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	ComponentRegistrar * registrar = new ComponentRegistrar();
	AutoConfigurator * a = new AutoConfigurator(registrar, "FileConfigurationProvider", "", "monitoring/activity_multiplexer/plugins/anomaly-skeleton/test/siox.conf");

	// AnomalySkeletonOptions * op = (AnomalySkeletonOptions*) ap->get_options();
	// ap->init(op, m1, facade);


	vector<Component*> components = a->LoadConfiguration("skeletonTest", "");
	cout << "Loaded: " << components.size() << " Components/Plugins" << endl;
	for(auto itr = components.begin() ; itr != components.end(); itr++){
		cout << *itr << endl;
	}


	ActivityMultiplexer * m1 = dynamic_cast<ActivityMultiplexer*>(components[0]);

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


