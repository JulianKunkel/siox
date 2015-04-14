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


/**
* @brief Metadata container, hides memory management. 
*
* REMOVE this class, when metadata can be aquired from DB
*/
class Metadata {
	public:
		// ctor
		Metadata(
				AssociationMapper* mapper,
				Ontology* ontology,
				SystemInformationGlobalIDManager* sysInfo) :
			m_mapper(mapper),
			m_ontology(ontology),
			m_sysInfo(sysInfo) {} //ctor
		
		// ctor
		Metadata(const std::string hostaddress,
				const std::string username,
				const std::string password,
				const std::string dbname,
				unsigned int port) 
		{
			Topology* topology = core::module_create_instance<Topology>("", "siox-monitoring-DatabaseTopology", MONITORING_TOPOLOGY_INTERFACE);

			topology->getOptions<DatabaseTopologyOptions>().hostaddress = hostaddress;
			topology->getOptions<DatabaseTopologyOptions>().username = username;
			topology->getOptions<DatabaseTopologyOptions>().password = password;
			topology->getOptions<DatabaseTopologyOptions>().dbname = dbname;
			topology->getOptions<DatabaseTopologyOptions>().port = port;
			topology->init();
			topology->start();

			m_mapper = core::module_create_instance<AssociationMapper>( 
					"", 
					"siox-monitoring-TopologyAssociationMapper", 
					MONITORING_ASSOCIATION_MAPPER_INTERFACE);
			m_mapper->getOptions<TopologyAssociationMapperOptions>().topology = topology;
			m_mapper->init();
			m_mapper->start();

			m_ontology = core::module_create_instance<Ontology>(
					"", 
					"siox-monitoring-TopologyOntology", 
					ONTOLOGY_INTERFACE);
			m_ontology->getOptions<TopologyOntologyOptions>().topology = topology;
			m_ontology->init();  
			m_ontology->start();   

			m_sysInfo = core::module_create_instance<SystemInformationGlobalIDManager>( 
					"", 
					"siox-monitoring-TopologySystemInformation", 
					SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE);
			m_sysInfo->getOptions<TopologySystemInformationOptions>().topology = topology;
			m_sysInfo->init();
			m_sysInfo->start();
		} // ctor 


		AssociationMapper* mapper() const {return m_mapper;}
		Ontology* ontology() const {return m_ontology;}
		SystemInformationGlobalIDManager* sysInfo() const {return m_sysInfo;}


		// dtor
		~Metadata() {
			if (m_mapper != nullptr) {
				delete m_mapper;
				m_mapper = nullptr;
			}
			if (m_ontology != nullptr) {
				delete m_ontology;
				m_ontology = nullptr;
			}
			if (m_sysInfo != nullptr) {
				delete m_sysInfo;
				m_sysInfo = nullptr;
			}
		} // dtor

	private:
		AssociationMapper* m_mapper;
		Ontology* m_ontology;
		SystemInformationGlobalIDManager* m_sysInfo;
};




/*
 This little program provides a command line interface to reading siox-traces.
 */
int main( int argc, char ** argv )
{
	try {
		program_options::options_description genericOptions( "Synopsis" );
		genericOptions.add_options()
		( "help", "This help message" )
		( "verbosity", program_options::value<int>()->default_value(0), "Verbosity level, 1 is Warning, 3+ is Debug" )
		( "conf", program_options::value<string>()->default_value( "" ), "Configuration file" )
		( "afile", program_options::value<string>()->default_value( "" ), "Activity file" )
		( "ofile", program_options::value<string>()->default_value( "ontology.dat" ), "Ontology file" )
		( "sfile", program_options::value<string>()->default_value( "system-info.dat" ), "System information file" )
		( "Afile", program_options::value<string>()->default_value( "association.dat" ), "Association file" )
		( "metadata", program_options::value<string>()->default_value( "file" ), "Meta data source. (file, db)" )
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
		vector<Component*> coreComponents = configurator->LoadConfiguration("TraceReader", "");
		std::list<ActivityMultiplexer*> amuxs = configurator->searchForAll<ActivityMultiplexer>(coreComponents);
		tools::ActivityInputStreamPlugin* activities = configurator->searchFor<tools::ActivityInputStreamPlugin>(coreComponents);
		assert(coreComponents.size() != 0);

		// fetch module options if available
		//for (Component* component : traceReaderComponents) {
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

		string file = vm["afile"].as<string>();
		if (file == ""){
			file = "activities.dat";
		}

		// set module options
		//for (Component* component : traceReaderComponents) {
		//	CommandLineOptions* opts = dynamic_cast<CommandLineOptions*>(component);
		//	if (opts) {
		//		opts->setOptions(vm);
		//	}
		//}

		AssociationMapper* associations           = nullptr;
		Ontology* ontology                        = nullptr;
		SystemInformationGlobalIDManager* sysInfo = nullptr;

		const std::string metadataSrc = vm["metadata"].as<std::string>();
		if (metadataSrc == "db") {
			Metadata metadata{"10.0.0.202", "siox", "siox", "siox", 5432};
			ontology     = metadata.ontology();
			associations = metadata.mapper();
			sysInfo      = metadata.sysInfo();
		}
		else if (metadataSrc == "file") {
			ontology     = configurator->searchFor<Ontology>(coreComponents);
			associations = configurator->searchFor<AssociationMapper>(coreComponents);
			sysInfo      = configurator->searchFor<SystemInformationGlobalIDManager>(coreComponents);
		}
		else {
			std::cerr << "Unknow activity meta data source" << metadataSrc << std::endl;
			exit(1);
		}

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

	} catch( std::exception & e ) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}	
