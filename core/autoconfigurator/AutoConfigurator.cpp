#include <mutex>

#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <core/module/module-loader.hpp>
#include <core/container/container-serializer.hpp>
#include <core/component/component-macros.hpp>


using namespace std;

using namespace core;

// store a global registrar while the data is deserialized
// this is a workaround, since deserialization templates of boost do not take additional arguments.
// 
// With Vala the whole code was much easier to understand, shorter and ways better to maintain.
ComponentRegistrar * autoConfiguratorRegistrar;
static mutex registrarMutex;


class LoadModule : public Container{
public:
	// name of the module
	std::string name;
	// path to search for the module
	std::string path;
	// required interface
	std::string interface;
	// component number, this is useful for references
	int componentID;

    SERIALIZE_CONTAINER(MEMBER(componentID) MEMBER(name) MEMBER(path) MEMBER(interface)) 
};
CREATE_SERIALIZEABLE_CLS(LoadModule)

namespace core{
/**
 * Describes the module to load in the configuration.
 */

	AutoConfigurator::~AutoConfigurator(){
		delete(configurationProvider);
	}

	AutoConfigurator::AutoConfigurator(ComponentRegistrar *r, string conf_provider_module, string module_path, string configuration_entry_point){
		configurationProvider = module_create_instance<ConfigurationProvider>(module_path, conf_provider_module, CONFIGURATION_PROVIDER_INTERFACE);
		configurationProvider->connect(configuration_entry_point);
		registrar = r;
	}

	AutoConfigurator::AutoConfigurator(ComponentRegistrar *r, ConfigurationProvider * conf_provider){
		registrar = r;
		string val = "";
		configurationProvider = conf_provider;
		configurationProvider->connect(val);
	}

	vector<Component*> AutoConfigurator::LoadConfiguration(string type, string matchingRules){		
		string config = configurationProvider->getConfiguration(type, matchingRules);

		vector<Component*> components;

		registrarMutex.lock();
		autoConfiguratorRegistrar = registrar;

		ContainerSerializer cs = ContainerSerializer();
		stringstream already_parsed_config(config);
		while(! already_parsed_config.eof()) {
			// check if the stream is empty
			char c; 
			already_parsed_config >> c;
			already_parsed_config.unget();
			if(!already_parsed_config.good())
				break;

			// no, so we hope that the configuration is complete
			LoadModule * module;
			ComponentOptions * options;
			Component * component;
	
			cout << "Parsing a module" << endl;
			module = dynamic_cast<LoadModule*>(cs.parse(already_parsed_config));

			cout << "Parsed module description" << endl;

			try{
				options = cs.parse(already_parsed_config);
			}catch(InvalidComponentException & e){
				cerr << "Error in configuration, the component " << e.invalidNr << " has not been found in configuration: " << endl << config << endl;
				throw e;
			}
			component = module_create_instance<Component>(module->path, module->name, module->interface);


			component->init(options);
			registrar->register_component(module->componentID, component);

			components.push_back(component);
	
			delete(module);
		}
		autoConfiguratorRegistrar = nullptr;
		registrarMutex.unlock();

		return components;
	}

	string AutoConfigurator::DumpConfiguration(ComponentOptions * options){
		ContainerSerializer cs = ContainerSerializer();
		string  str = cs.serialize(options);
		return str;
	}

}