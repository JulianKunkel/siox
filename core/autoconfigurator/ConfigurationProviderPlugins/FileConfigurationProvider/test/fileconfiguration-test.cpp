#include <assert.h>
#include <iostream>

#include <core/autoconfigurator/AutoConfigurator.hpp>

#include "test-serialize-modules.hpp"

using namespace core;

int main(){
	ComponentRegistrar * registrar = new ComponentRegistrar();
	// make test runable using waf
	if (chdir ("../../../../../../monitoring") == 0){
		chdir("../");
	}

	AutoConfigurator * a = new AutoConfigurator(registrar, "FileConfigurationProvider", "", "core/autoconfigurator/ConfigurationProviderPlugins/FileConfigurationProvider/test/test.config");

	MyChildModule * child = new MyChildModule();
	MyParentModule * parent = new MyParentModule();

	cout << "Parent Empty Configuration" << endl;
	cout << a->DumpConfiguration(parent->get_options()) << endl;

	cout << "Child Empty Configuration" << endl;
	cout << a->DumpConfiguration(child->get_options()) << endl;

	cout << "Loading configuration from file" << endl;
	vector<Component*> components = a->LoadConfiguration("daemon", "hostname=\"node1\" mode=\"debug\"");
	
	parent = dynamic_cast<MyParentModule*>(components[1]); 
	assert(parent);
	child = dynamic_cast<MyChildModule*>(components[0]);
	assert(child);

	assert(parent->options);
	assert(child->options);

	cout << parent->options->pname << endl;
	cout << child->options->name << endl;
	cout << parent->options->childInterface.componentID << endl;
	assert(parent->options->childInterface.componentID);

	assert(parent->options->childInterface.instance<MyChildModule>() == child);

	delete(registrar);

	return 0;
}
