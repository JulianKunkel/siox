#include <mutex>
#include <iostream>

#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <core/module/ModuleLoader.hpp>
#include <core/autoconfigurator/AutoConfigurator.hpp>

#include <core/autoconfigurator/ModuleOptionsXML.hpp>
#include <core/container/container-serializer.hpp>


using namespace std;

using namespace core;

// store a global registrar while the data is deserialized
// this is a workaround, since deserialization templates of boost do not take additional arguments.
//
// With Vala the whole code was much easier to understand, shorter and ways better to maintain.
ComponentRegistrar * autoConfiguratorRegistrar;
int autoConfiguratorOffset;
static mutex registrarMutex;

namespace core {
	/**
	 * Describes the module to load in the configuration.
	 */

	AutoConfigurator::~AutoConfigurator()
	{
		delete( configurationProvider );
	}

	AutoConfigurator::AutoConfigurator( ComponentRegistrar * r, string conf_provider_module, string module_path, string configuration_entry_point )
	{
		configurationProvider = module_create_instance<ConfigurationProvider>( module_path, conf_provider_module, CONFIGURATION_PROVIDER_INTERFACE );
		configurationProvider->connect( configuration_entry_point );
		registrar = r;
	}

	/*
	 Ownership of the conf_provider is given to the AutoConfigurator
	 */
	AutoConfigurator::AutoConfigurator( ComponentRegistrar * r, ConfigurationProvider * conf_provider )
	{
		registrar = r;
		string val = "";
		configurationProvider = conf_provider;
		configurationProvider->connect( val );
	}

	static void outputErrorPosition(int pos, stringstream & ss){
		const string & s = ss.str();
		cerr << "Configuration error " << endl << s.substr(0, pos) << "!!!!ERROR IS HERE!!!!" << s.substr(pos) << endl;
	}

	vector<Component *> AutoConfigurator::LoadConfiguration( string type, string matchingRules, bool initComponents ) throw( InvalidComponentException, InvalidConfiguration )
	{
		string config = configurationProvider->getConfiguration( type, matchingRules );
		if( config.length() < 10 ) {
			throw InvalidConfiguration( "Configuration is unavailable or too short", "" );
		}
		stringstream transformed_config;

		{
			stringstream input(config);
			
			// erase comment lines
		   string item;
	   	while (getline(input, item, '\n')) {
	   		if (item[0] == '#'){
	   			transformed_config << endl;
	   			continue;
	   		}
	   		transformed_config << item << endl;
	    	}
    	}
    	// cout << transformed_config.str() << endl;

		// parse string
		vector<Component *> components;

		registrarMutex.lock();
		autoConfiguratorOffset = registrar->getMaximumComponentNumber();
		autoConfiguratorRegistrar = registrar;

		stringstream already_parsed_config( transformed_config.str() );
		//cout << transformed_config.str() << endl;
		while( ! already_parsed_config.eof() ) {
			// check if the stream is empty
			char c;
			already_parsed_config >> c;
			already_parsed_config.unget();
			if( !already_parsed_config.good() )
				break;

			// no, so we hope that the configuration is complete
			LoadModule * module = new LoadModule();
			Component * component;


			int pos = already_parsed_config.tellg();

			//cout << "Parsing a module" << endl;			
			try{
				j_xml_serialization::deserialize( *module, already_parsed_config );
			} catch( exception & e ) {
				autoConfiguratorRegistrar = nullptr;
				registrarMutex.unlock();

				cerr << "Error: " << e.what() << endl;
				outputErrorPosition(pos, already_parsed_config);
				throw InvalidConfiguration( "Error while parsing module configuration", "");
			}

			pos = already_parsed_config.tellg();
			cout << "[AC] Module config: "  << module->name << " (" << module->interface << ")" << endl;
			component = module_create_instance<Component>( module->path, module->name, module->interface );
			//cout << DumpConfiguration(component->get_options()) << endl;
			//try{				
			try{
				SetConfiguration( component, already_parsed_config );
			}catch(InvalidConfiguration & e){
				outputErrorPosition(pos, already_parsed_config);
				throw InvalidConfiguration( "Error during parsing of options", module->name);
			}

			registrar->registerComponent( module->componentID + autoConfiguratorOffset, type + " " + matchingRules, module->name, component );

			components.push_back( component );

			delete( module );
		}
		autoConfiguratorRegistrar = nullptr;
		registrarMutex.unlock();

		if (initComponents){
			initAllComponents(components);
		}
		return components;
	}

	void AutoConfigurator::SetConfiguration(Component * component, const string & config) throw (InvalidConfiguration){
		stringstream s(config);
		SetConfiguration(component, s);
	}

	void AutoConfigurator::SetConfiguration(Component * component, stringstream & config) throw (InvalidConfiguration){
		ComponentOptions * options = & component->getOptions();
		ContainerSerializer cs;
		cs.parse( options, config );
		try {
			//j_xml_serialization::deserialize( *options, config );
		} catch( exception & e ) {
			autoConfiguratorRegistrar = nullptr;
			registrarMutex.unlock();

			cerr << endl << "Expected configuration: " << DumpConfiguration(options) << endl;
			throw InvalidConfiguration( "Error during parsing of options", "unknown");
		}
	}

	void AutoConfigurator::initAllComponents(vector<Component *> & c){
		for (auto it = c.begin(); it != c.end(); it++){
			try {
				(*it)->init();				
			} catch( exception & e ) {
				autoConfiguratorRegistrar = nullptr;
				registrarMutex.unlock();

				string  str = DumpConfiguration( & (*it)->getOptions());
				cerr << "Configuration values: " << endl << str << endl;
				throw InvalidConfiguration( string( "Error during initialization: " ) + e.what(), "unknown" );
			}

		}
	}


	string AutoConfigurator::DumpConfiguration( ComponentOptions * options )
	{
		ContainerSerializer cs;
		return cs.serialize(options);
	}

}