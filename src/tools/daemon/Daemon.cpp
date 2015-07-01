#include <signal.h>

#include <exception>
#include <iostream>
#include <condition_variable>
#include <mutex>

#include <core/persist/SetupPersistentStructures.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <util/autoLoadModules.hpp>
#include <util/ReporterHelper.hpp>

using namespace std;

static ComponentRegistrar * registrar;
// Make sure the daemon quits when it receives SIGINT.
static mutex finish_mutex;
static condition_variable finish_condition;
static bool terminated = false;


void signalHandler(int sig){
	cout << "Received signal " << sig << endl;

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
		boost::program_options::options_description desc( "Synopsis" );
		desc.add_options()
		( "help", "This help message :-)" )
		( "d", "Daemonize")
		( "configEntry", boost::program_options::value<string>(& configEntry), "Starting point for requesting a configuration")
		( "configProvider", boost::program_options::value<string>(& configProvider), "Configuration provider module")
		( "configSection", boost::program_options::value<string>(&configSection), "Configuration section to use")
		( "createPersistentStructures", "Instruct all loaded daemon plugins to create the required structures to persist data for subsequent operations" )
#ifndef PRODUCTIVE_DEPLOYMENT
		( "purgePersistentStructures", "Instruct all loaded daemon plugins to purge the currently stored persistent storages" )
#endif
		;

		boost::program_options::variables_map vm;
		boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), vm );
		boost::program_options::notify( vm );

		if( vm.count( "help" ) ) {
			cout << desc << endl;
			return 0;
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

		if ( vm.count ("createPersistentStructures") || vm.count ("purgePersistentStructures") ){
			const bool create = vm.count ("createPersistentStructures");

			cout << "Creating persistent structures" << endl;

			list< pair<SetupPersistentStructures*, RegisteredComponent*> > lst = registrar->listAllComponentsOfATypeFull<SetupPersistentStructures>();
			for( auto c = lst.begin(); c != lst.end(); c++ ){		
				cout << c->second->toString() << " ";
				int ret = 0;
				if (create){
					ret = c->first->preparePersistentStructuresIfNecessary();
#ifndef PRODUCTIVE_DEPLOYMENT					
				}else{
					ret = c->first->cleanPersistentStructures();
#endif
				}				
				if (ret){
					cout << " [OK]"  << endl;
				}else{
					cout << " [ERROR]" << endl;
				}
			}
			cout << "Done creating persistent structures" << endl;

			if (lst.size() == 0){
				cout << "WARNING: No supported plugin requested to persist anything. Maybe the configuration is wrong?" << endl;
			}

			registrar->shutdown();
			return 0;
		}

		// Stop on SIGINT, SIGTERM and SIGUSR1 (USR1 eases debugging)
		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGUSR1, signalHandler);


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

		registrar->start();

		// wait until SIGINT is received
		while(! terminated){
			unique_lock<mutex> lock(finish_mutex);
			finish_condition.wait(lock);
		}

		registrar->stop();

		// shutdown operation
		util::invokeAllReporters( registrar );

		registrar->shutdown();
	} catch( std::exception & e ) {
		cerr << e.what() << endl;
		return 1;
	}
	return EXIT_SUCCESS;
}
