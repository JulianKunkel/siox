#include <unistd.h>

#include <vector>
#include <iostream>

#include <util/autoLoadModules.hpp>

namespace util{

string getHostname(){
		char local_hostname[1024];
		local_hostname[1023] = '\0';
		gethostname( local_hostname, 1023 );
		return local_hostname ;
}

vector<Component *> LoadConfiguration(AutoConfigurator ** outConfigurator, ComponentRegistrar * registrar){
		string hostname = getHostname();

		const char * provider = getenv( "SIOX_CONFIGURATION_PROVIDER_MODULE" );
		const char * path = getenv( "SIOX_CONFIGURATION_PROVIDER_PATH" );
		const char * configuration = getenv( "SIOX_CONFIGURATION_PROVIDER_ENTRY_POINT" );

		provider = ( provider != nullptr ) ? provider : "siox-core-autoconfigurator-FileConfigurationProvider" ;
		path = ( path != nullptr ) ? path : "";
		configuration = ( configuration != nullptr ) ? configuration :  "siox.conf:/usr/local/etc/siox.conf:/etc/siox.conf:monitoring/low-level-c/test/siox.conf:monitoring/low-level-c/test/siox.conf" ;

		AutoConfigurator * configurator = new AutoConfigurator( registrar, provider, path, configuration );
		*outConfigurator = configurator;

		const char * configurationMode = getenv( "SIOX_CONFIGURATION_PROVIDER_MODE" );
		const char * configurationOverride = getenv( "SIOX_CONFIGURATION_SECTION_TO_USE" );

		string configName;
		if( configurationOverride != nullptr ) {
			configName = configurationOverride;
		} else {
			// hostname configurationMode (is optional)

			{
				stringstream configName_s;
				configName_s << hostname;
				if( configurationMode != nullptr ) {
					configName_s << " " << configurationMode;
				}
				configName = configName_s.str();
			}
		}

		cout << provider << " path " << path << " " << configuration << " ConfigName: \"" << configName << "\"" << endl;

		vector<Component *> loadedComponents;
		try {
			loadedComponents = configurator->LoadConfiguration( "Process", configName );
		} catch( InvalidConfiguration & e ) {
			// fallback to global configuration
			loadedComponents = configurator->LoadConfiguration( "Process", "" );
		}


		// if(loadedComponents == nullptr){ // MZ: Error, "==" not defined
		if( loadedComponents.empty() ) {
			cerr << "FATAL Invalid configuration set: " << endl;
			cerr << "SIOX_CONFIGURATION_PROVIDER_MODULE=" << provider << endl;
			cerr << "SIOX_CONFIGURATION_PROVIDER_PATH=" << path << endl;
			cerr << "SIOX_CONFIGURATION_PROVIDER_ENTRY_POINT=" << configuration << endl;
			cerr << "SIOX_CONFIGURATION_PROVIDER_MODE=" << configurationMode << endl;
			// TODO use FATAL function somehow?
			exit( 1 );
		}
		return loadedComponents;
	}
}