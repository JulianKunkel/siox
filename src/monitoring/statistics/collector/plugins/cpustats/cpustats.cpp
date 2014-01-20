#include "../ProcSingleFilePlugin.hpp"

#include <map>
#include <stdint.h>
#include <unistd.h>

using namespace std;

/*
Reads from /proc/stats and parses the fields.

cpu  1001327 1535 211910 10043870 10999 10 11650 0 0 0
cpu0 367697 622 75475 2366969 3437 2 5201 0 0 0
cpu1 340882 532 77640 2383862 1543 6 6175 0 0 0
cpu2 142832 236 26710 2653559 4581 0 189 0 0 0
cpu3 149915 144 32083 2639479 1436 1 84 0 0 0
intr 23060279 43 1607 0 0 0 0 0 0 1 701 0 0 40894 0 0 0 436195 161 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 14198 4736161 340745 1180523 531 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 124918753
btime 1374908428
processes 21508
procs_running 1
procs_blocked 0
softirq 10934790 0 3455549 2149 25126 326929 0 1222230 3122564 25616 2754627

See http://www.linuxhowtos.org/System/procstat.htm for a description.

*/

class CPUstats: public ProcSingleFilePlugin<12> {
		enum CPUfields {
			USER, NICE, SYSTEM, IDLE, IOWAIT, IRQ, SOFTIRQ, VIRTUAL_SYSTEMS, VIRTUAL_OS
		};

		long tickLen;
	protected:
		vector<vector<StatisticsValue> > CPUValues;
		StatisticsValue interrupts;
		StatisticsValue sw_interrupts;
		StatisticsValue contextSwitches;
		StatisticsValue threadsCreated;
		StatisticsValue curProcsRunning;
		StatisticsValue curProcsBlocked;

		void timestepLine( int lineNr, vector<string> & entries ) {
			string name = entries[0];

			if( name.compare( 0, 3, "cpu" ) == 0 ) {
				int cpu;
				if( name.length() == 3 ) {
					// aggregate
					cpu = 0;
				} else {
					cpu = atoi( name.c_str() + 3 ) + 1;
				}

				vector<StatisticsValue> & vec = CPUValues[cpu];
				for( unsigned i = 1; i < entries.size() ; i++ ) {
					vec[i - 1] = ( uint64_t ) atoll( entries[i].c_str() ) * tickLen;
				}
			} else if( name == "intr" ) {
				interrupts = ( uint64_t ) atoll( entries[1].c_str() );
			} else if( name == "ctxt" ) {
				contextSwitches = ( uint64_t ) atoll( entries[1].c_str() );
			} else if( name == "processes" ) {
				threadsCreated = ( uint64_t ) atoll( entries[1].c_str() );
			} else if( name == "procs_running" ) {
				curProcsRunning = ( uint64_t ) atoll( entries[1].c_str() );
			} else if( name == "procs_blocked" ) {
				curProcsBlocked = ( uint64_t ) atoll( entries[1].c_str() );
			} else if( name == "softirq" ) {
				sw_interrupts = ( uint64_t ) atoll( entries[1].c_str() );
			}
		}

		void initLine( int lineNr, vector<string> & entries ) {
			string name = entries[0];
			if( name.compare( 0, 3, "cpu" ) == 0 ) {
				CPUValues.push_back( vector<StatisticsValue>( entries.size() - 1 ) );
			}
		}

		const string filename() {
			return "/proc/stat";
		}

		virtual vector<StatisticsProviderDatatypes> availableMetrics() {
			vector<StatisticsProviderDatatypes> result;

			uint64_t overflow_value = ( uint64_t ) 1 << 63; //TODO CHECK ME, we expect 64 Bit...

			// compute the duration of a tick in ms.
			tickLen = 1000 / sysconf( _SC_CLK_TCK );

			for( unsigned i = 0; i < CPUValues.size(); i++ ) {
				vector<StatisticsValue> & vals = CPUValues[i];

				StatisticsScope type = NODE;
				vector<pair<string, string> > topology {{"node", "@localhost"}};

				if( i > 0 ) {
					type = DEVICE;
					char deviceName[4];
					snprintf( deviceName, 4, "%d", i - 1 );
					topology.push_back( {"device", deviceName} );
				}

				result.push_back( {CPU, type, "time/user", "@localhost", vals[USER] , INCREMENTAL, "ms", "Time spend in user mode", overflow_value, 0} );
				result.push_back( {CPU, type, "time/nice", "@localhost", vals[NICE] , INCREMENTAL, "ms", "Time spend for niced processes", overflow_value, 0} );
				result.push_back( {CPU, type, "time/system", "@localhost", vals[SYSTEM] , INCREMENTAL, "ms", "Time spend in system mode", overflow_value, 0} );
				result.push_back( {CPU, type, "time/idle", "@localhost", vals[IDLE] , INCREMENTAL, "ms", "Idle time", overflow_value, 0} );
				result.push_back( {CPU, type, "time/iowait", "@localhost", vals[IOWAIT] , INCREMENTAL, "ms", "Time spend waiting for IO", overflow_value, 0} );
				result.push_back( {CPU, type, "time/interrupts", "@localhost", vals[IRQ] , INCREMENTAL, "ms", "Time spend for IRQ", overflow_value, 0} );
				result.push_back( {CPU, type, "time/softirq", "@localhost", vals[SOFTIRQ] , INCREMENTAL, "ms", "Time spend for softIRQ ", overflow_value, 0} );
				result.push_back( {CPU, type, "time/vms", "@localhost", vals[VIRTUAL_SYSTEMS] , INCREMENTAL, "ms", "Time spend executing virtual hosts", overflow_value, 0} );
				result.push_back( {CPU, type, "time/vmsOS", "@localhost", vals[VIRTUAL_OS] , INCREMENTAL, "ms", "Time spend for guest operating systems", overflow_value, 0} );
			}

			result.push_back( {OS, NODE, "quantity/hwInterrupts", "@localhost", interrupts, INCREMENTAL, "", "Serviced interrupts", overflow_value, 0} );
			result.push_back( {OS, NODE, "quantity/swInterrupts", "@localhost", sw_interrupts , INCREMENTAL, "", "Serviced software interrupts", overflow_value, 0} );
			result.push_back( {OS, NODE, "quantity/contextSwitches", "@localhost", contextSwitches , INCREMENTAL, "", "The total number of context switches across all CPUs.", overflow_value, 0} );
			result.push_back( {OS, NODE, "quantity/threadsCreated", "@localhost", threadsCreated , INCREMENTAL, "", "The number of processes and threads created", overflow_value, 0} );
			result.push_back( {OS, NODE, "quantity/procs/running", "@localhost", curProcsRunning , SAMPLED, "", "The number of processes currently running on all CPUs.", 0, 0} );
			result.push_back( {OS, NODE, "quantity/procs/blocked", "@localhost", curProcsBlocked , SAMPLED, "", "The the number of processes currently blocked.", 0, 0} );

			return result;
		}
};

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new CPUstats();
	}
}
