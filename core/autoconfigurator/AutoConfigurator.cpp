#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <core/module/module-loader.hpp>
#include <core/container/container-serializer.hpp>


using namespace std;

namespace core{

	AutoConfigurator::~AutoConfigurator(){
		delete(configurationProvider);
	}

	AutoConfigurator::AutoConfigurator(string conf_provider_module, string module_path, string configuration_entry_point){
		configurationProvider = module_create_instance<ConfigurationProvider>(module_path, conf_provider_module, CONFIGURATION_PROVIDER_INTERFACE);
		configurationProvider->connect(configuration_entry_point);
	}

	AutoConfigurator::AutoConfigurator(ConfigurationProvider * conf_provider){
		string val = "";
		configurationProvider = conf_provider;
		configurationProvider->connect(val);
	}

 	/**
 	* The parent component is expected to be created but still not configured...
 	*/
	void AutoConfigurator::LoadConfiguration(Component & parent_component, string type, map<string, string> & optional_matchings_priority){
		ComponentOptions * options = parent_component.get_options();
		string config = configurationProvider->getConfiguration(type, optional_matchings_priority);
		// TODO fill stuff using ConfigurationProvider
		parent_component.init(options);
	}

	string AutoConfigurator::DumEmptyConfiguration(Component & parent_component){
		ComponentOptions * options = parent_component.get_options();

		ContainerSerializer * cs = new ContainerSerializer();
		string  str = cs->serialize(options);
		delete(options);

		return str;
	}

}