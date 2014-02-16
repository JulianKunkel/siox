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
#include <util/ExceptionHandling.hpp>

#include <vector>

namespace {

	using namespace std;
	using namespace monitoring;

	class LikwidPlugin : public StatisticsProviderPlugin {
		public:
			void init( StatisticsProviderPluginOptions & options ) override;
			void finalize() override;

			void nextTimestep() throw() override;
			vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
		private:
			EventSetup likwidSetup;
			// results from likwid
			float * values;
			vector<StatisticsValue> statisticsValues;
	};

	void LikwidPlugin::finalize(void){
		perfmon_stopCounters();
		perfmon_finalize();
	}

	void LikwidPlugin::init( StatisticsProviderPluginOptions& options ) {

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

		// TODO support multiple groups...
		likwidSetup = perfmon_prepareEventSetup("VIEW");

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

	vector<StatisticsProviderDatatypes> LikwidPlugin::availableMetrics() throw() {
		vector<StatisticsProviderDatatypes> result;

		#define addGaugeMetric( name, variableName, unitString, descriptionString ) \
			result.push_back( {StatisticsEntity::CPU, StatisticsScope::NODE, name, "@localhost", variableName, GAUGE, unitString, descriptionString, 0, 0} ); 
		
		for( int i=0; i < likwidSetup.numberOfDerivedCounters; i++ ){			
			// extract correct unit from likwid
			char * name;
			const char * fnd = strstr(likwidSetup.derivedNames[i], " [");
			char * unit;
			if (fnd){
				int len = fnd - likwidSetup.derivedNames[i] + 1;
				name = strndup(likwidSetup.derivedNames[i], len);
				name[len-1] = 0;
				unit = strdup(fnd + 2);
				unit[strlen(unit) - 1] = 0;
			}else{
				unit = "";
				name = (char*) likwidSetup.derivedNames[i];
			}
			// TODO use correct name for the metric.
			addGaugeMetric( name, statisticsValues[i], unit, likwidSetup.derivedNames[i]);

			if (name != likwidSetup.derivedNames[i]){
				free(name);
				free(unit);
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
