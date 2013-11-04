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
			vector<StatisticsProviderDatatypes> lst;

			uint64_t overflow_value = ( uint64_t ) 1 << 63; //TODO CHECK ME, we expect 64 Bit...

			for( auto itr = values.begin(); itr != values.end(); itr++ ) {
				char name[51];
				name[50] = 0;
				snprintf( name, 50, "quantity/%s", itr->first.c_str() );

				string unit = itr->second.second == "kB" ?  "Bytes" : itr->second.second;

				lst.push_back( {MEMORY, NODE, name, {{"node", LOCAL_HOSTNAME}}, itr->second.first, SAMPLED, unit, "", overflow_value, 0} );
			}

			return lst;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new OSMemUsage();
	}
}
