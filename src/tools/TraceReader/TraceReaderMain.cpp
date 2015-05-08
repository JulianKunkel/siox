#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

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

#include "AdjustXML.cpp"

/*
 This little program provides a command line interface to reading siox-traces.
 */
int main( int argc, char ** argv )
{
	//try {
		program_options::options_description genericOptions( "Synopsis" );
		genericOptions.add_options()
		( "helpGeneral", "This help message" )
		( "help", "The module-specific help" )
		( "verbosity", program_options::value<int>()->default_value(0), "Verbosity level, 1 is Warning, 3+ is Debug" )
		( "conf", program_options::value<string>()->default_value( "" ), "Configuration file" )
		( "ignoreConfigOptions", "Ignore module options")
		;

		program_options::variables_map vm;
		program_options::store( program_options::command_line_parser(argc, argv).options(genericOptions).allow_unregistered().style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm );
		program_options::notify( vm );

		if( vm.count( "helpGeneral" ) ) {
			cout << genericOptions << endl;
			return 1;
		}


		string configFile = vm["conf"].as<string>();
		if (configFile == ""){
			configFile = "siox-trace-reader.conf:/etc/siox/trace-reader.conf";
		}

		const bool ignoreConfigOptions = vm.count("ignoreConfigOptions") > 0;

		program_options::options_description cmdline_options;


		// fetch module options if available and add them to the cmdline_options

		// Loading Modules according to configuration file.
		ComponentRegistrar registrar{};
		AutoConfigurator* configurator = new AutoConfigurator(
				& registrar, "siox-core-autoconfigurator-FileConfigurationProvider",	"", configFile); 
		vector<Component*> coreComponents = configurator->LoadConfiguration("TraceReader", "", false);
		std::list<ActivityMultiplexer*> amuxs = configurator->searchForAll<ActivityMultiplexer>(coreComponents);
		tools::ActivityInputStreamPlugin* activities = configurator->searchFor<tools::ActivityInputStreamPlugin>(coreComponents);
		assert(coreComponents.size() != 0);
	
		if (! ignoreConfigOptions){
			unordered_map<string,int> identicalNames;

			for (Component* c : coreComponents) {
				// parse options that can be set from XML...
				// change the XML and inject it back.
				string config = configurator->DumpConfiguration(c);
	
				//cout << config << endl;
				stringstream s(config);
				bool isFinishTag;
				vector<string> stack= {};
	
				string txt;
				bool finishedLast = false;

	         while(s.good()){
	         	// extract next begin tag
	         	string tag = tools::XML::extractNextTag(s, isFinishTag);
	         	if (isFinishTag){         		
	         		string last = stack.back();
	         		stack.pop_back();	
	
	         		if (tag == last && stack.size() > 0 && finishedLast == false && txt.size() > 0){
	         			stringstream val;
	         			for(string parent : stack ){
	         				val << parent << ".";
	         			}
	         			val << last;
							string optname = val.str();

							if (identicalNames.find(optname) == identicalNames.end()){
								identicalNames[optname] = 1;
							}else{
								val << identicalNames[optname];
								identicalNames[optname]++;
								optname = val.str();
							}

	         			// no nested tags => potential candidate
	         			// cout << val.str() << " " << txt << endl;
	         			cmdline_options.add_options()( optname.c_str(), program_options::value<string>()->default_value( txt ));
	         		}
	         		txt = "";
	         		finishedLast = true;
	         		continue;
	         	}
	         	finishedLast = false;
	         	txt = tools::XML::extractText(s);

	         	if (stack.empty()){
		         	if (identicalNames.find(tag) == identicalNames.end()){
							identicalNames[tag] = 1;
	         		}else{
	         			stringstream tmp;
	         			tmp << tag;
	         			tmp << identicalNames[tag];
	         			identicalNames[tag]++;
	         			tag = tmp.str();
	         		}
	         	}
	         	stack.push_back(tag);
	         }
			}
		}

		// the final command line options
		cmdline_options.add(genericOptions);

		// now all options can be parsed
		program_options::store( program_options::command_line_parser(argc, argv).options(cmdline_options).style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm);
		program_options::notify( vm );

		if( vm.count( "help" ) ) {
			cout << cmdline_options << endl;
			return 1;
		}

		if (! ignoreConfigOptions){
			unordered_map<string,int> identicalNames;

			// set module options
			for (Component* c : coreComponents) {
				// parse options that can be set from XML...
				// change the XML and inject it back.
				string config = configurator->DumpConfiguration(c);
				stringstream newConfig;
	
				stringstream s(config);
				bool isFinishTag;
				vector<string> stack= {};
	
				string txt;
				bool finishedLast = false;
	
	         while(s.good()){
	         	// extract next begin tag
	         	string tag = tools::XML::extractNextTag(s, isFinishTag);
	         	if (isFinishTag){
	         		string last = stack.back();
	         		stack.pop_back();	
	
	         		if (tag == last && stack.size() > 0 && finishedLast == false && txt.size() > 0){
	         			stringstream val;
	         			for(string parent : stack ){
	         				val << parent << ".";
	         			}
	         			val << last;

							string optname = val.str();

							if (identicalNames.find(optname) == identicalNames.end()){
								identicalNames[optname] = 1;
							}else{
								val << identicalNames[optname];
								identicalNames[optname]++;
								optname = val.str();
							}
	
	         			// no nested tags => potential candidate         			
	     					string txtVal = vm[optname].as<string>();
	     					//cout << val.str() << " " << txtVal << txt << endl;
	     					newConfig << txtVal;     					
	         		}else{
	         			newConfig << txt;
	         		}
	         		txt = "";
	         		finishedLast = true;
	
	         		newConfig << "</" << tag << ">" << endl;
	         		continue;
	         	}
	         	if (! s.good()){
	         		break;
	         	}
	         	newConfig << "<" << tag << ">";
	
	         	finishedLast = false;
	         	txt = tools::XML::extractText(s);

	         	if (stack.empty()){
		         	if (identicalNames.find(tag) == identicalNames.end()){
							identicalNames[tag] = 1;
	         		}else{
	         			stringstream tmp;
	         			tmp << tag;
	         			tmp << identicalNames[tag];
	         			identicalNames[tag]++;
	         			tag = tmp.str();	         		}
	         	}
	         	stack.push_back(tag);
	         }
	
	  			//cout << newConfig.str() << endl;
	  			//cout << config << endl;
  				configurator->SetConfiguration(c, newConfig.str());	
			}
		}

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
