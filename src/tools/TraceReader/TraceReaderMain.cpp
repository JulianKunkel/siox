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
		( "plugin", program_options::value<string>()->default_value( "Print" ), "invoke the specified trace reader plugin")
		;

		program_options::variables_map vm;
		program_options::store( program_options::basic_command_line_parser<char>(argc, argv).options(genericOptions).allow_unregistered().run(), vm );
		program_options::notify( vm );

		string plainName = vm["plugin"].as<string>();
		string module = "siox-tools-TraceReader-" + plainName + "Plugin" ;
		TraceReaderPlugin * plugin = core::module_create_instance<TraceReaderPlugin>( "", module, TRACE_READER_PLUGIN_INTERFACE );

		// add command line options
		program_options::options_description descPlugin( plainName + " plugin options" );
		plugin->moduleOptions(descPlugin);


		program_options::options_description cmdline_options;
		cmdline_options.add(genericOptions).add(descPlugin);

		if( vm.count( "help" ) ) {
			cout << cmdline_options << endl;
			return 1;
		}

		// now all options can be parsed
		program_options::store( program_options::parse_command_line( argc, argv, cmdline_options ), vm );
		program_options::notify( vm );

		TraceReader tr = TraceReader( vm["afile"].as<string>(), vm["sfile"].as<string>(), vm["ofile"].as<string>(), vm["Afile"].as<string>() );


		// init plugin
		TraceReaderOptions & tro = plugin->getOptions<TraceReaderOptions>();
		tro.traceReader = & tr;
		tro.vm = & vm;
		tro.verbosity = vm["verbosity"].as<int>();
		plugin->init();

		while( true ) {
			Activity * a = tr.nextActivity();
			if( a == nullptr ){
				break;
			}
			plugin->nextActivity(a);
		}

		plugin->finalize();

		delete(plugin);
	} catch( std::exception & e ) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}