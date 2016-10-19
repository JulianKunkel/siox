/**
 * @file ./src/monitoring/statistics/collector/test/WrapAroundTestDefault.cpp
 *
 * @author Nathanael Hübbe
 * @date   2014
 */

#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>

#include <monitoring/statistics/Statistic.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

#include "../../../../monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp"
#include "../../collector/ThreadedStatisticsOptions.hpp"


using namespace core;
using namespace monitoring;

class ProviderDummy : public StatisticsProviderPlugin {
	public:
		ProviderDummy() {
			metric = (uint64_t)42;
			cout << "ProviderDummy: Setting metric to " << metric << "\n";
		}
		virtual void nextTimestep() throw() {
			metric += (uint64_t)1 << 62;
			cout << "ProviderDummy: Setting metric to " << metric << "\n";
		}
		virtual vector<StatisticsProviderDatatypes> availableMetrics() throw() {
			vector<StatisticsProviderDatatypes> result;
			result.push_back(StatisticsProviderDatatypes("foo", "@localhost", metric, INCREMENTAL, "bar", "baz"));
			return result;
		}
	private:
		StatisticsValue metric;
};

class MuxDummy : public StatisticsMultiplexer {
	public:
		virtual ComponentOptions* AvailableOptions() {
			ComponentOptions* options = new ComponentOptions();
			return options;
		}
		virtual void init() {}
		virtual void notifyAvailableStatisticsChange( const std::vector<std::shared_ptr<Statistic> > & statistics, bool addedStatistics, bool removedStatistics ) throw() override {
			assert(!myStatistic);
			if(statistics.size()) myStatistic = statistics[0];
		}
		virtual void newDataAvailable() throw() override {
			if(myStatistic) {
				cout << "MuxDummy: myStatistic == " << myStatistic->curValue << "\n";
				if(gotFirstValue) {
					assert(myStatistic->curValue == (uint64_t)1 << 62);
				}
				gotFirstValue = true;
			}
		}
		virtual void registerListener( StatisticsMultiplexerListener* listener ) throw() override {}
		virtual void unregisterListener( StatisticsMultiplexerListener* listener ) throw() override {}
	private:
		std::shared_ptr<Statistic> myStatistic;
		bool gotFirstValue = false;
};

int main( int argc, char const * argv[] ) throw() {
	#define makeModule(Type, PointerName, ModuleName, InterfaceName) \
		Type* PointerName = module_create_instance<Type>( "", ModuleName, InterfaceName)
	makeModule( Topology, topology, "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	makeModule( ActivityPluginDereferencing, facade, "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE );
	makeModule( Ontology, ontology, "siox-monitoring-TopologyOntology", ONTOLOGY_INTERFACE );
	makeModule( StatisticsCollector, collector, "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );
	ProviderDummy* provider = new ProviderDummy();
	MuxDummy* mux = new MuxDummy();

	#define fetchOptions(Type, ComponentVariableName) \
		Type* ComponentVariableName##Options = &ComponentVariableName->getOptions<Type>();
	fetchOptions( ComponentOptions, topology );
	fetchOptions( DereferencingFacadeOptions, facade );
	fetchOptions( TopologyOntologyOptions, ontology );
	fetchOptions( ThreadedStatisticsOptions, collector );
	fetchOptions( StatisticsProviderPluginOptions, provider );
	fetchOptions( ComponentOptions, mux );

	(void)topologyOptions;	//avoid warning about unused variable
	facadeOptions->topology.componentPointer = topology;
	ontologyOptions->topology.componentPointer = topology;
	collectorOptions->topology = topology;
	collectorOptions->ontology.componentPointer = ontology;
	collectorOptions->smux.componentPointer = mux;
	providerOptions->statisticsCollector = collector;
	(void)muxOptions;	//avoid warning about unused variable

	topology->init();
	ontology->init();
	facade->init();
	mux->init();
	collector->init();
	collector->start();
	provider->init();

	cerr << "sleeping\n";
	sleep( 1 );
	cerr << "waking up\n";

	collector->stop();
	delete collector;
	delete ontology;
	delete topology;

	cerr << argv[0] << ": OK" << endl;
	return 0;
}

