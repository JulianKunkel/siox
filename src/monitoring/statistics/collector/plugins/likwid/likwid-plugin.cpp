/**
 * @author JK
 * @date 2014
 */
#include <stdio.h>

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
			void init( StatisticsProviderPluginOptions * options ) override;
			void finalize() override;

			ComponentOptions * AvailableOptions() override{
				return new ProviderLikwidOptions();
			}

			void nextTimestep() throw() override;
			vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
		private:
			void likwidDerivedEventToOntology(const char * likwidName, char const ** outName, char const ** outUnit);

			EventSetup likwidSetup;
			// results from likwid
			float * values;
			vector<StatisticsValue> statisticsValues;
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
		int * threads = (int*) malloc(sizeof(int) * numThreads);
		for(int i=0; i < numThreads; i++){
			threads[i] = i;
		}

		// 0 or 1 at the moment
		accessClient_setaccessmode(1);

		FILE * nullFD = fopen("/dev/null", "w");
		perfmon_init(numThreads, threads, nullFD);
		
		int groupCount = options->groups.length() > 0 ? 1 : 0;
		const char * cur = options->groups.c_str();
		while( *cur != '\0'){
			if ( *cur == ',') groupCount++;
			cur++;
		}

		// create all groups
		// TODO support multiple groups properly
		assert(groupCount == 1);
		likwidSetup = perfmon_prepareEventSetup( (char*) options->groups.c_str());

		values = (float*) malloc( likwidSetup.numberOfDerivedCounters * sizeof(float) * 3);
		statisticsValues.resize(likwidSetup.numberOfDerivedCounters);
		for( int i=0; i < likwidSetup.numberOfDerivedCounters; i++ ){
			statisticsValues[i] = 0.0f;
		}		

		perfmon_setupCountersForEventSet(& likwidSetup);
		perfmon_startCounters();		
	}

	void LikwidPlugin::nextTimestep() throw() {
		perfmon_stopCounters();
		
		perfmon_getDerivedCounterValues(& values[0], & values[likwidSetup.numberOfDerivedCounters], & values[likwidSetup.numberOfDerivedCounters*2]);	

		// copy likwid results
		for( int i=0; i < likwidSetup.numberOfDerivedCounters; i++ ){
			statisticsValues[i] = values[i];
		}

		perfmon_startCounters();
	}

	void LikwidPlugin::likwidDerivedEventToOntology(const char * likwidName, char const ** outName, char const ** outUnit){
		struct LikwidType{
			const char * Lname;
			const char * unit;
			const char * name;
		};

		LikwidType types[] = {
			{"Runtime (RDTSC) [s]", "s", "time/cpu/Runtime (RDTSC)"},
			{"Runtime unhalted [s]", "s", "time/cpu/RuntimeUnhalted"},
			{"Clock [MHz]", "MHz", "throughput/cpu/Clockspeed"},
			{"CPI", "", "throughput/cpu/CPI"},
			{"DP MFlops/s (DP assumed)", "MFlops/s", "throughput/cpu/DPFlops"},
			{"SP MFlops/s (SP assumed)", "MFlops/s", "throughput/cpu/SPFlops"},
			{"Packed MUOPS/s", "MUOPS/s", "throughput/cpu/PackedMUOPS"},
			{"Scalar MUOPS/s", "MUOPS/s", "throughput/cpu/ScalarMUOPS"},
			{"SP MUOPS/s", "MUOPS/s", "throughput/cpu/SPMUOPS"},
			{"DP MUOPS/s", "MUOPS/s", "throughput/cpu/DPMUOPS"},
			{"Memory bandwidth [MBytes/s]", "MBytes/s", "throughput/memory/bandwidth"},
			{"Memory data volume [GBytes]", "GBytes", "quantity/memory/volume"},
			{"Remote Read BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteReadBW"},
			{"Remote Write BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteWriteBW"},
			{"Remote BW [MBytes/s]", "MBytes/s", "throughput/memory/RemoteBW"},
			{"Energy [J]", "J", "energy/Socket/rapl"},
			{"Energy DRAM [J]", "J", "energy/DRAM/rapl"},
			{"Power [W]", "W", "power/rapl"},
			{nullptr, nullptr, nullptr} };
		const LikwidType * check_type = types;

		//FIXME: Undefined behaviour when the likwidName is not found in the list (*outName and *outUnit remain uninitialized).
		while(check_type->Lname != nullptr){
			if (strcmp(check_type->Lname, likwidName) == 0){
				*outName = check_type->name;
				*outUnit = check_type->unit;
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


		for( int i=0; i < likwidSetup.numberOfDerivedCounters; i++ ){			
			// extract correct unit from likwid
			const char * name;
			const char * unit;
			likwidDerivedEventToOntology(likwidSetup.derivedNames[i], & name, & unit);

			// TODO use correct name for the metric.
			//if (strcmp(name, "energy/Socket/rapl") == 0 ){			
			addGaugeMetric( name, statisticsValues[i], unit, likwidSetup.derivedNames[i]);
		}

		return result;
	}
}

extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME() {
		return new LikwidPlugin();
	}
}
