#include <core/autoconfigurator/ConfigurationProviderImplementation.hpp>
#include <core/container/container-serializer.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <exception>

#include <assert.h>

using namespace core;
using namespace std;


class FileConfigurationProvider : public ConfigurationProvider {
	public:

		void connect( string & configuration_filename ) throw( ConfigurationProviderError ) {
			// try for existance of potentially multiple files multiple configuration_filename are separated by ":" or tab.

			vector<string> filelist;
			boost::split( filelist, configuration_filename, boost::is_any_of( ":\t" ) );

			ifstream fin;
			for( auto it = filelist.begin(); it != filelist.end(); ++it ) {
				fin.open( *it );
				//cout << "> Checking config " << *it << endl;
				if( fin.good() ) {
					cout << "> config file: " << *it << endl;
					break;
				}
			}
			if( ! fin.good() ) {
				throw ConfigurationProviderError( "Error configuration file(s) (" + configuration_filename + ") are not existing/readable" );
			}

			string line;
			stringstream data;
			string configSection;
			while( getline( fin, line ) ) {
				if( line.length() < 3 ) {
					continue;
				}
				if( line.compare( 0, 7, "#CONFIG" ) == 0 ) {
					parseData( configSection, data.str() );
					data.str( "" );
					configSection = line;
				} else {
					data << line << endl;
				}
			}
			parseData( configSection, data.str() );

			fin.close();
		}

		const string & getConfiguration( string & type, string & matchingRules ) throw( ConfigurationProviderError ) {
			string what = type + ( matchingRules.length() > 0 ? " " + matchingRules : "" );
			return configurationSections[what];
		}

	private:
		// this map contains the key <type, all optional info concatenated using "/"> and value: configuration
		map<string, string> configurationSections;

		void parseData( string & configSection, string && data ) {
			if( configSection.length() < 4 ) {
				return;
			}
			configSection = configSection.substr( 8 ); // strip: #CONFIG
			configurationSections[configSection] = data;
		}
};

<<<<<<< HEAD
extern "C" {
	void* CONFIGURATION_PROVIDER_INSTANCIATOR_NAME() { return new FileConfigurationProvider(); }
}
=======
COMPONENT( FileConfigurationProvider )

>>>>>>> master
