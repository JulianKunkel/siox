#include <exception>
#include <iostream>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <core/module/ModuleLoader.hpp>
#include <util/autoLoadModules.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/association_mapper/modules/FileAssociationMapper/FileAssociationMapperOptions.hpp>
#include <monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp>
#include <monitoring/system_information/modules/filebased-system-information/FileBasedSystemInformationOptions.hpp>

#include <monitoring/topology/Topology.hpp>
#include <monitoring/topology/databaseTopology/DatabaseTopologyOptions.hpp>                                        
#include <monitoring/association_mapper/modules/TopologyAssociationMapper/TopologyAssociationMapperOptions.hpp>    
#include <monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp>                                
#include <monitoring/system_information/modules/TopologySystemInformation/TopologySystemInformationOptions.hpp>    

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

//#include <knowledge/optimizer/Optimizer.hpp>

//#include <tools/TraceReader/activity_stream/FileAIStream/FileAIStream.hpp>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPlugin.hpp>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPluginOptions.hpp>

using namespace boost;
using namespace std;
using namespace monitoring;


/*
 This little program provides a command line interface to reading siox-traces.
 */
int main( int argc, char ** argv )
{
	//try {
		program_options::options_description genericOptions( "Synopsis" );
		genericOptions.add_options()
		( "help", "This help message" )
		( "verbosity", program_options::value<int>()->default_value(0), "Verbosity level, 1 is Warning, 3+ is Debug" )
		( "conf", program_options::value<string>()->default_value( "" ), "Configuration file" )
		( "module", program_options::value<string>()->default_value( "" ), "Set module options")
		;

		program_options::variables_map vm;
		program_options::store( program_options::command_line_parser(argc, argv).options(genericOptions).allow_unregistered().style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm );
		program_options::notify( vm );

		string configFile = vm["conf"].as<string>();

		if( vm.count( "help" ) and configFile == "" ) {
			cout << genericOptions << endl;
			return 1;
		}

		if (configFile == ""){
			configFile = "siox-trace-reader.conf:/etc/siox/trace-reader.conf";
		}

		// Loading Modules
		ComponentRegistrar registrar{};
		AutoConfigurator* configurator = new AutoConfigurator(
				& registrar, "siox-core-autoconfigurator-FileConfigurationProvider",	"", configFile); 
		vector<Component*> coreComponents = configurator->LoadConfiguration("TraceReader", "", false);
		std::list<ActivityMultiplexer*> amuxs = configurator->searchForAll<ActivityMultiplexer>(coreComponents);
		tools::ActivityInputStreamPlugin* activities = configurator->searchFor<tools::ActivityInputStreamPlugin>(coreComponents);
		assert(coreComponents.size() != 0);

		// fetch module options if available
		for (Component* c : coreComponents) {
			// parse options that can be set from XML...
			// change the XML and inject it back.
			// configurator->SetConfiguration(c, configurator->DumpConfiguration(c));
		}
		//	CommandLineOptions* opts = dynamic_cast<CommandLineOptions*>(component);
		//	if (opts) {
		//		opts->moduleOptions(genericOptions);
		//	}
		//}

		// the final command line options
		program_options::options_description cmdline_options;
		cmdline_options.add(genericOptions);

		// now all options can be parsed
		program_options::store( program_options::command_line_parser(argc, argv).options(cmdline_options).style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm);
		program_options::notify( vm );

		if( vm.count( "help" ) ) {
			cout << cmdline_options << endl;
			return 1;
		}

		// set module options
		//for (Component* component : traceReaderComponents) {
		//	CommandLineOptions* opts = dynamic_cast<CommandLineOptions*>(component);
		//	if (opts) {
		//		opts->setOptions(vm);
		//	}
		//}

		// initialize all modules with the appropriate module options
		configurator->initAllComponents(coreComponents);

		AssociationMapper* associations           = nullptr;
		Ontology* ontology                        = nullptr;
		SystemInformationGlobalIDManager* sysInfo = nullptr;

		ontology     = configurator->searchFor<Ontology>(coreComponents);
		associations = configurator->searchFor<AssociationMapper>(coreComponents);
		sysInfo      = configurator->searchFor<SystemInformationGlobalIDManager>(coreComponents);

		assert(ontology);
		assert(associations);
		assert(sysInfo);

		registrar.start();
	
		while(const auto activity = activities->nextActivity()) {
			for (auto& amux : amuxs) {
				amux->Log(activity);
			}
		}

		registrar.stop();
		registrar.shutdown();
		delete configurator; // TODO: make smart AutoConfigurator, to get rid of "delete"

	//} catch( std::exception & e ) {
	//	cerr << e.what() << endl;
	//	return 1;
	//}
	return 0;
}	
