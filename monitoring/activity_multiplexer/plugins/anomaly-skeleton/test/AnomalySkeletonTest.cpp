#include <core/module/module-loader.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/Ontology.hpp>

#include "../AnomalySkeletonOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	ComponentRegistrar registrar = ComponentRegistrar();
	AutoConfigurator * a = new AutoConfigurator(& registrar, "FileConfigurationProvider", "", "monitoring/activity_multiplexer/plugins/anomaly-skeleton/test/siox.conf");

	vector<Component*> components = a->LoadConfiguration("skeletonTest", "");
	cout << "Loaded: " << components.size() << " Components/Plugins" << endl;
	for(auto itr = components.begin() ; itr != components.end(); itr++){
		cout << *itr << endl;
	}

	// the activity multiplexer is expected to be first:
	ActivityMultiplexer * m1 = dynamic_cast<ActivityMultiplexer*>(components[0]);
	Ontology * o = dynamic_cast<Ontology*>(components[1]);
	SystemInformationGlobalIDManager * sys = dynamic_cast<SystemInformationGlobalIDManager*>(components[3]);	

	UniqueInterfaceID uid = sys->interface_id("test", "impl1");
    UniqueComponentActivityID aid = sys->activity_id(uid, "open");
    OntologyAttribute * o1 = o->register_attribute("test", "filesize", SIOX_STORAGE_64_BIT_UINTEGER);
    OntologyAttribute * o2 = o->register_attribute("test", "filename", SIOX_STORAGE_STRING);
    OntologyAttribute * o3 = o->register_attribute("test", "filesystem", SIOX_STORAGE_32_BIT_UINTEGER);

	ComponentID cid = {.pid = {2,3,4}, .num=1};

	auto parentArray = vector<ActivityID>{{{.pid = {1,2,3}, .num=2}, .num = 1} };
	auto attributeArray = vector<Attribute>();
	attributeArray.push_back({.id= o2->aID, .value = "julian/data.nc4"});
	attributeArray.push_back({.id= o1->aID, .value = (uint64_t) 4711});
	attributeArray.push_back({.id= o3->aID, .value = (uint32_t) 1});

	auto remoteCallsArray = vector<RemoteCall>();

	cout << attributeArray.size() << endl;

	// Cast the real activity to the serializable object class wrapper
	Activity * activity = new Activity(aid, 3, 5, cid, parentArray, attributeArray, remoteCallsArray, nullptr, 0);

	m1->Log(activity);

	registrar.shutdown();

	cout << "OK" << endl;

	delete(a);
	
	return 0;
}


