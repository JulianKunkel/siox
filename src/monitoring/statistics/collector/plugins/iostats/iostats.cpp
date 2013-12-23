#include "../ProcSingleFilePlugin.hpp"
#include <stdint.h>
#include <stdio.h>

#include <map>

class IOstats: public ProcSingleFilePlugin<15> {
	protected:

		void timestepLine( int lineNr,  vector<string> & entries ) {
			string name = entries[2];

			if( currentValues.find( name ) == currentValues.end() ) {
				cerr << "File " << filename() << " changed while accessing. New field " << name << endl;
				return;
			}

			std::array<StatisticsValue, 11> & cur = currentValues[name];

			for( int i = 0; i < 2; i++ ) {
				cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
			}
			cur[2] = ( ( uint64_t ) atoll( entries[2 + 3].c_str() ) ) * 512;

			for( int i = 3; i < 6; i++ ) {
				cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
			}
			cur[6] = ( ( uint64_t ) atoll( entries[6 + 3].c_str() ) ) * 512;

			for( int i = 7; i < 11; i++ ) {
				cur[i] = ( uint64_t ) atoll( entries[i + 3].c_str() );
			}
		}

		void initLine( int lineNr, vector<string> & entries ) {
			string name = entries[2];
			currentValues[name] = std::array<StatisticsValue, 11>();
		}

		const string filename() {
			return "/proc/diskstats";
		}

		map<string, std::array<StatisticsValue, 11> > currentValues;


	public:

		virtual vector<StatisticsProviderDatatypes> availableMetrics() {
			vector<StatisticsProviderDatatypes> result;
			// walk through all known devices:

			// TODO: determine available DEVICES using /sys/block/
			// Could be combined using the library for detecting hardware in the KB?
			// Right now we consider DEVICES to be NODE metrics, although this is wrong.
			// It is NODE level for virtual devices and real DEVICES for existing hardware

			for( auto iterator = currentValues.begin(); iterator != currentValues.end(); iterator++ ) {
				string name = iterator->first;
				string topologyPath = string( "@topology/" ) + name;
				//cout << name << endl;
				//Add the 11 metrics
				uint64_t overflow_value = ( uint64_t ) 1 << 63; //TODO CHECK ME, we expect 64 Bit...

				std::array<StatisticsValue, 11> & cur = currentValues[name];
				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/reads", topologyPath, cur[0], INCREMENTAL, "", "Field 1 -- # of reads issued", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/reads/merged", topologyPath, cur[1], INCREMENTAL, "", "Field 2 -- # of reads merged", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/dataRead", topologyPath, cur[2], INCREMENTAL, "Bytes", "Data read based on Field 3 -- # of sectors read", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "time/block/reads", topologyPath, cur[3], INCREMENTAL, "ms", "Field 4 -- # of milliseconds spent reading", overflow_value, 0} );

				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/writes", topologyPath, cur[4], INCREMENTAL, "", "Field 5 -- # of writes completed", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/writes/merged", topologyPath, cur[5], INCREMENTAL, "", "Field 6 -- # of writes merged", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/dataWritten", topologyPath, cur[6], INCREMENTAL, "Bytes", "Data written based on Field 7 -- # of sectors written", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "time/block/writes", topologyPath, cur[7], INCREMENTAL, "ms", "Field 8 -- # of milliseconds spent writing", overflow_value, 0} );

				result.push_back( {INPUT_OUTPUT, NODE, "quantity/block/pendingIOs", topologyPath, cur[8], SAMPLED, "", "Field 9 -- # of I/Os currently in progress", 0, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "time/block/access", topologyPath, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os", overflow_value, 0} );
				result.push_back( {INPUT_OUTPUT, NODE, "time/block/weighted", topologyPath, cur[10], INCREMENTAL, "ms", "Field 11 -- weighted # of milliseconds spent doing I/Os", overflow_value, 0} );
			}

			return result;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new IOstats();
	}
}
