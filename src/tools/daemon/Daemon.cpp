#include <signal.h>

#include <exception>
#include <iostream>
#include <condition_variable>
#include <mutex>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <util/autoLoadModules.hpp>
#include <util/ReporterHelper.hpp>


using namespace boost;
using namespace std;



static ComponentRegistrar * registrar;
// Make sure the daemon quits when it receives SIGINT.
static mutex finish_mutex;
static condition_variable finish_condition;
static bool terminated = false;


void signalHandler(int sig){
	cout << "Shutting down daemon" << endl;

	util::invokeAllReporters( registrar );
	registrar->shutdown();

	lock_guard<mutex> lock(finish_mutex);
	terminated = true;
	finish_condition.notify_one();
}

/*
 This little program provides a command line interface to reading siox-traces.
 */
int main( int argc, char ** argv )
{
	try {
		// variables for command line arguments
		string configEntry;
		string configProvider;
		string configSection;

		// parse command line arguments
		program_options::options_description desc( "Synopsis" );
		desc.add_options()
		( "help", "This help message :-)" )
		( "d", "Daemonize")
		( "configEntry", program_options::value<string>(& configEntry), "Starting point for requesting a configuration")
		( "configProvider", program_options::value<string>(& configProvider), "Configuration provider module")
		( "configSection", program_options::value<string>(&configSection), "Configuration section to use")
		;

		program_options::variables_map vm;
		program_options::store( program_options::parse_command_line( argc, argv, desc ), vm );
		program_options::notify( vm );

		if( vm.count( "help" ) ) {
			cout << desc << endl;
			return 1;
		}

		// override environment variables if set as parameter
		if (configEntry != "")
			setenv( "SIOX_CONFIGURATION_PROVIDER_ENTRY_POINT", configEntry.c_str(), 1 );
		if (configProvider != "")
			setenv( "SIOX_CONFIGURATION_PROVIDER_MODULE", configProvider.c_str(), 1 );
		if (configSection != "")
			setenv( "SIOX_CONFIGURATION_SECTION_TO_USE", configSection.c_str(), 1 );

      // load configuration 
		registrar = new ComponentRegistrar();
		AutoConfigurator * configurator = nullptr;
		vector<Component *> loadedComponents = util::LoadConfiguration(& configurator, registrar);
		signal(SIGINT, signalHandler);

		// fork daemon if requested:
		if (vm.count("d")){
			pid_t pid; 
			pid = fork();
			
			if (pid < 0) {
			    exit(EXIT_FAILURE);
			}
			if (pid > 0) { 
				// the parent process can terminate
			    exit(EXIT_SUCCESS);
			}

	        /* Close the standard file descriptors */
	        close(STDIN_FILENO);
	        close(STDOUT_FILENO);
	        close(STDERR_FILENO);
		}

		// wait until SIGINT is received
		while(! terminated){
			unique_lock<mutex> lock(finish_mutex);
			finish_condition.wait(lock);
		}
	} catch( std::exception & e ) {
		cerr << e.what() << endl;
		return 1;
	}
	return EXIT_SUCCESS;
}
