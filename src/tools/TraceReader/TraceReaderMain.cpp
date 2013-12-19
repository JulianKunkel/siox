#include <exception>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <core/module/ModuleLoader.hpp>

#include <tools/TraceReader/TraceReader.hpp>
#include <tools/TraceReader/TraceReaderPlugin.hpp>

using namespace boost;
using namespace std;
using namespace tools;

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
		( "afile", program_options::value<string>()->default_value( "activities.dat" ), "Activity file" )
		( "ofile", program_options::value<string>()->default_value( "ontology.dat" ), "Ontology file" )
		( "sfile", program_options::value<string>()->default_value( "system-info.dat" ), "System information file" )
		( "Afile", program_options::value<string>()->default_value( "association.dat" ), "Association file" )
		( "plugin", program_options::value<vector<string> >()->multitoken(), "invoke the specified trace reader plugin, default=Print")
		;

		program_options::variables_map vm;
		program_options::store( program_options::command_line_parser(argc, argv).options(genericOptions).allow_unregistered().style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm );
		program_options::notify( vm );

		vector<string> pluginNames;
		if ( ! vm.count("plugin") ){
			pluginNames.push_back("Print");
		}else{
			pluginNames = vm["plugin"].as< vector<string> >();					
		}

		// the final command line options
		program_options::options_description cmdline_options;
		cmdline_options.add(genericOptions);

		// load all plugins and initialize them
		vector<TraceReaderPlugin*> plugins;
		for( auto itr = pluginNames.begin(); itr != pluginNames.end(); itr++ ){
			string plainName = *itr;
			string module = "siox-tools-TraceReader-" + plainName + "Plugin" ;
			TraceReaderPlugin * plugin = core::module_create_instance<TraceReaderPlugin>( "", module, TRACE_READER_PLUGIN_INTERFACE );

			// add command line options
			program_options::options_description descPlugin( plainName + " plugin options" );
			plugin->moduleOptions(descPlugin);
			cmdline_options.add(descPlugin);

			plugins.push_back(plugin);
		}

		if( vm.count( "help" ) ) {
			cout << cmdline_options << endl;
			return 1;
		}

		// now all options can be parsed
		program_options::store( program_options::command_line_parser(argc, argv).options(cmdline_options).style(program_options::command_line_style::unix_style ^ program_options::command_line_style::allow_short).run(), vm);
		program_options::notify( vm );

		TraceReader tr = TraceReader( vm["afile"].as<string>(), vm["sfile"].as<string>(), vm["ofile"].as<string>(), vm["Afile"].as<string>() );

		for( auto itr = plugins.begin(); itr != plugins.end(); ){
			auto plugin = *itr;
			// init plugin
			TraceReaderOptions & tro = plugin->getOptions<TraceReaderOptions>();
			tro.traceReader = & tr;
			tro.vm = & vm;
			tro.verbosity = vm["verbosity"].as<int>();

			// build the chain of plugins
			++itr;
			if ( itr != plugins.end() ){
				tro.nextPlugin = *itr;
			}else{
				tro.nextPlugin = nullptr;
			}

			plugin->init();
		}

		// loop through all activities
		TraceReaderPlugin * firstPlugin = plugins[0];
		while( true ) {
			Activity * a = tr.nextActivity();
			if( a == nullptr ){
				break;
			}
			firstPlugin->nextActivity(a);
		}

		// finalize all plugins
		for( auto itr = plugins.begin(); itr != plugins.end(); itr++ ){
			auto plugin = *itr;
			plugin->finalize();
			delete(plugin);
		}
	} catch( std::exception & e ) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}