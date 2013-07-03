#ifndef SIOX_AUTOCONFIGURATOR_H
#define SIOX_AUTOCONFIGURATOR_H

#include <core/autoconfigurator/ConfigurationProvider.hpp>
#include <core/component/Component.hpp>
#include <core/component/ComponentRegistrar.hpp>

#include <vector>

/*
 * Since this class does provide/parse the configuration it does not need to be derived from Component.
 */

using namespace std;

namespace core{

class AutoConfigurator{
public:
	/*
	 * This constructur is used for fully automatic configuration.
	 * i.e. when the configuration provider will load/know its primary configuration and load new configurations manually.
	 * This is mainly used in application processes
	 *
	 * configuration_entry_point is module specific and allows the module to fetch information from this entry point, e.g. a file.
	 * This is useful, for example, if the user sets a file_configuration upon startup of the daemon.
	 */
	AutoConfigurator(ComponentRegistrar *r, string conf_provider_module, string module_path, string configuration_entry_point);

	/*
	 * This constructur allows to inject a ConfigurationProvider with its own settings.
	 * Mainly for testing purpose.	 
	 */
	AutoConfigurator(ComponentRegistrar *r, ConfigurationProvider * conf_provider);

	~AutoConfigurator();

	/*
	 * Load/Parse the configuration options, create the necessary components and link their attributes.
	 */
	vector<Component*> LoadConfiguration(string type, string matchingRules);

	/*
	 * Create an empty configuration entry for the given component
	 */
	string DumpConfiguration(ComponentOptions * options);

private:	
	ConfigurationProvider * configurationProvider;
	ComponentRegistrar * registrar;
 };


}

#endif
