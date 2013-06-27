#ifndef SIOX_AUTOCONFIGURATOR_H
#define SIOX_AUTOCONFIGURATOR_H

#include <core/autoconfigurator/ConfigurationProvider.hpp>
#include <core/component/Component.hpp>

//#include <core/module/module-loader.hpp>

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
	AutoConfigurator(string conf_provider_module, string module_path, string configuration_entry_point);

	/*
	 * This constructur allows to inject a ConfigurationProvider with its own settings.
	 * Mainly for testing purpose.	 
	 */
	AutoConfigurator(ConfigurationProvider * conf_provider);

	~AutoConfigurator();

	/*
	 * Load/Parse the configuration options into the options object.
	 */
	void LoadConfiguration(Component & parent_component, string type, map<string, string> & optional_matchings_priority);

	string DumEmptyConfiguration(Component & parent_component);

private:	
	ConfigurationProvider * configurationProvider;
 };


}

#endif
