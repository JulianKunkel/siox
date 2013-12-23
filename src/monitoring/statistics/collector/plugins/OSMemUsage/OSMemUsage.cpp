#include "../ProcSingleFilePlugin.hpp"

#include <map>
#include <utility>

using namespace std;

/*
Reads from /proc/meminfo and parses the fields.
MemTotal:        3908064 kB
...
HugePages_Rsvd:        0
*/

class OSMemUsage: public ProcSingleFilePlugin<3> {
	protected:
		map<string, pair<StatisticsValue, string> > values;

		string initRegex() {
			return "[ \t]*[^ \t:]+";
		}

		void timestepLine( int lineNr, vector<string> & entries ) {
			string name = entries[0];
			uint64_t value = ( uint64_t ) atoll( entries[1].c_str() );

			if( values.find( name ) == values.end() ) {
				cerr << "File " << filename() << " changed while accessing. New field " << name << endl;
				return;
			}

			auto & v = values[name];

			if( v.second == "kB" ) {
				v.first = value * 1024;
			} else {
				v.first = value;
			}
		}

		void initLine( int lineNr, vector<string> & entries ) {
			string name = entries[0];
			string unit = "";
			if( entries.size() >= 3 ) {
				unit = entries[2];
			}

			values[name] = {( uint64_t ) 0, unit};
		}

		const string filename() {
			return "/proc/meminfo";
		}

		virtual vector<StatisticsProviderDatatypes> availableMetrics() {
			vector<StatisticsProviderDatatypes> result;

			uint64_t overflow_value = ( uint64_t ) 1 << 63; //TODO CHECK ME, we expect 64 Bit...

			for( auto iterator = values.begin(); iterator != values.end(); iterator++ ) {
				string ontologyName = string( "quantity/" ) + iterator->first;
				string unit = iterator->second.second == "kB" ?  "Bytes" : iterator->second.second;
				result.push_back( {MEMORY, NODE, ontologyName, "@localhost", iterator->second.first, SAMPLED, unit, "", overflow_value, 0} );
			}

			return result;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new OSMemUsage();
	}
}
