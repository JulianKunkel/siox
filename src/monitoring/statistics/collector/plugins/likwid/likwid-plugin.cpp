/**
 * @author JK
 * @date 2014
 */
#include <stdio.h>
#include <string.h>

extern "C"{
#include <likwid/perfmon.h>
#include <likwid/cpuid.h>
#include <likwid/numa.h>
#include <likwid/accessClient.h>
#include <likwid/timer.h>
}

#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>
#include <monitoring/statistics/collector/plugins/likwid/ProviderLikwidOptions.hpp>
#include <util/ExceptionHandling.hpp>

#include <vector>

namespace {

	using namespace std;
	using namespace core;
	using namespace monitoring;


	class LikwidPlugin : public StatisticsProviderPlugin {
		public:
			struct ObservedType{
				const char * unit;
				const char * name;
				bool shouldBeAccumulated;
				StatisticsValue value = 0.0f;
			};

			struct GroupData{
					EventSetup likwidSetup;
					// results from likwid
					float * values;
					vector<ObservedType> statistics;
			};

			void init( StatisticsProviderPluginOptions * options ) override;
			void finalize() override;

			ComponentOptions * AvailableOptions() override{
				return new ProviderLikwidOptions();
			}

			void nextTimestep() throw() override;
			vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
		private:
			void likwidDerivedEventToOntology(const char * likwidName, char const ** outName, char const ** outUnit, bool * shouldBeAccumulated);

			vector<GroupData> groups;
			unsigned currentGroup = 0;

			int numCores;
	};

	void LikwidPlugin::finalize(void){
		perfmon_stopCounters();
		perfmon_finalize();
	}

	void LikwidPlugin::init( StatisticsProviderPluginOptions * sp_options ) {
		ProviderLikwidOptions * options = dynamic_cast<ProviderLikwidOptions*>( sp_options);

		if (cpuid_init() == EXIT_FAILURE){
			assert(FALSE && "CPUID_INIT");
		}
		numa_init();
		timer_init();


		const int numThreads = cpuid_topology.numSockets * cpuid_topology.numCoresPerSocket * cpuid_topology.numThreadsPerCore;
		numCores = numThreads;
		int * threads = (int*) malloc(sizeof(int) * numThreads);
		for(int i=0; i < numThreads; i++){
			threads[i] = i;
		}

		// 0 or 1 at the moment
		accessClient_setaccessmode(1);

		FILE * nullFD = fopen("/dev/null", "w");
		perfmon_init(numThreads, threads, nullFD);

		// create all groups
		char * str = (char*) options->groups.c_str();
		char * saveptr = nullptr;
		str = strtok_r(str, ",", & saveptr);
      while (str != nullptr){
      	groups.push_back({});
      	struct GroupData * group = & groups[groups.size()-1];

			group->likwidSetup = perfmon_prepareEventSetup( str );

			group->values = (float*) malloc( group->likwidSetup.numberOfDerivedCounters * sizeof(float) * 3);
			group->statistics.resize(group->likwidSetup.numberOfDerivedCounters);

			for( int i=0; i < group->likwidSetup.numberOfDerivedCounters; i++ ){			
				// extract correct unit from likwid
				likwidDerivedEventToOntology(group->likwidSetup.derivedNames[i], & group->statistics[i].name, & group->statistics[i].unit, & group->statistics[i].shouldBeAccumulated);
			}
			//cout << "CREATING : " << & group->likwidSetup << endl;
      	str = strtok_r(nullptr, ",", & saveptr);
		}

		perfmon_setupCountersForEventSet(& groups.begin()->likwidSetup);
		perfmon_startCounters();
	}

	void LikwidPlugin::nextTimestep() throw() {
		perfmon_stopCounters();

		struct GroupData * group = & groups[currentGroup];
		//cout << "ACCESSING : " << currentGroup << " " << & group->likwidSetup << endl;
		
		perfmon_setupCountersForEventSet(& group->likwidSetup);
		cout << currentGroup << " " << groups.size() << endl;
		currentGroup = (currentGroup + 1) % groups.size();

		perfmon_getDerivedCounterValues(& group->values[0], & group->values[group->likwidSetup.numberOfDerivedCounters], & group->values[group->likwidSetup.numberOfDerivedCounters*2]);	
		
		// copy likwid results
		for( int i=0; i < group->likwidSetup.numberOfDerivedCounters; i++ ){
			if ( group->statistics[i].shouldBeAccumulated ){				
				group->statistics[i].value = group->values[i] * numCores;
				//cout << statistics[i].name << " " << statistics[i].value << " " << numCores << endl;
			}else{
				group->statistics[i].value = group->values[i];
			}
		}

		perfmon_startCounters();
	}

	void LikwidPlugin::likwidDerivedEventToOntology(const char * likwidName, char const ** outName, char const ** outUnit, bool * shouldBeAccumulated){
		struct LikwidType{
			const char * Lname;
			const char * unit;
			const char * name;
			bool shouldBeAccumulated;
		};

		LikwidType types[] = {
			{"Runtime (RDTSC) [s]", "s", "time/cpu/Runtime (RDTSC)", false},
			{"Runtime unhalted [s]", "s", "time/cpu/UnhaltedConsumed", true},
			{"Clock [MHz]", "MHz", "throughput/cpu/Clockspeed", false},
			{"CPI", "", "throughput/cpu/CPI", false},
			{"DP MFlops/s (DP assumed)", "MFlops/s", "throughput/cpu/DPFlops", false},
			{"SP MFlops/s (SP assumed)", "MFlops/s", "throughput/cpu/SPFlops", false},
			{"Packed MUOPS/s", "MUOPS/s", "throughput/cpu/PackedMUOPS", false},
			{"Scalar MUOPS/s", "MUOPS/s", "throughput/cpu/ScalarMUOPS", false},
			{"SP MUOPS/s", "MUOPS/s", "throughput/cpu/SPMUOPS", false},
			{"DP MUOPS/s", "MUOPS/s", "throughput/cpu/DPMUOPS", false},
			{"Memory bandwidth [MBytes/s]", "MBytes/s", "throughput/memory/bandwidth", false},
			{"L2 Load [MBytes/s]", "MBytes/s", "throughput/L2/load", false},
			{"L2 Evict [MBytes/s]", "MBytes/s", "throughput/L2/evict", false},
			{"L2 bandwidth [MBytes/s]", "MBytes/s", "throughput/L2/bandwidth", false},
			{"L2 data volume [GBytes]", "GBytes", "quantity/L2/volume", true},
			{"Memory data volume [GBytes]", "GBytes", "quantity/memory/volume", true},
			{"Remote Read BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteReadBW", false},
			{"Remote Write BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteWriteBW", false},
			{"Remote BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteBW", false},
			{"Energy [J]", "J", "energy/Socket/rapl", true},
			{"Energy DRAM [J]", "J", "energy/DRAM/rapl", true},
			{"Power [W]", "W", "power/rapl", true},
			{nullptr, nullptr, nullptr} };
		const LikwidType * check_type = types;

		//FIXME: Undefined behaviour when the likwidName is not found in the list (*outName and *outUnit remain uninitialized).
		while(check_type->Lname != nullptr){
			if (strcmp(check_type->Lname, likwidName) == 0){
				*outName = check_type->name;
				*outUnit = check_type->unit;
				*shouldBeAccumulated = check_type->shouldBeAccumulated;
				return;
			}
			check_type++;
		}

		cerr << "LikwidPlugin: Warning do not understand event: " << likwidName << endl;
	}

	vector<StatisticsProviderDatatypes> LikwidPlugin::availableMetrics() throw() {
		vector<StatisticsProviderDatatypes> result;

		#define addGaugeMetric( name, variableName, unitString, descriptionString ) \
			result.push_back( {name, "@localhost", variableName, GAUGE, unitString, descriptionString, 0, 0} ); 
		#define addIncrementalMetric( name, variableName, unitString, descriptionString ) \
			result.push_back( {name, "@localhost", variableName, INCREMENTAL, unitString, descriptionString, 0, 0} );

		for( auto group = groups.begin(); group != groups.end(); group++ ){
			for( int i=0; i < group->likwidSetup.numberOfDerivedCounters; i++ ){			
				// TODO use correct name for the metric.	
				addGaugeMetric( group->statistics[i].name, group->statistics[i].value, group->statistics[i].unit, group->likwidSetup.derivedNames[i]);
			}
		}

		return result;
	}
}

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME() {
		return new LikwidPlugin();
	}
}
