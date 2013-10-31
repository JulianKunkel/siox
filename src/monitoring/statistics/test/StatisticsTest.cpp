/**
 * @file StatisticsTest.cpp
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <monitoring/statistics/collector/Statistic.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include "../multiplexer/plugins/testListener/TestListener.hpp"
#include <monitoring/ontology/Ontology.hpp>

#include "../../../monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp"
#include "../collector/plugins/provider-skeleton/ProviderSkeletonOptions.hpp"
#include "../collector/ThreadedStatisticsOptions.hpp"
#include "../multiplexer/StatisticsMultiplexerSyncOptions.hpp"
#include "../multiplexer/plugins/testListener/TestListenerOptions.hpp"


using namespace core;
using namespace monitoring;

int main( int argc, char const * argv[] ) throw() {
	#define makeModule(Type, PointerName, ModuleName, InterfaceName) \
		Type* PointerName = module_create_instance<Type>( "", ModuleName, InterfaceName)
	makeModule( Ontology, ontology, "siox-monitoring-FileOntology", ONTOLOGY_INTERFACE );
	makeModule( StatisticsProviderPlugin, provider, "siox-monitoring-statisticsPlugin-providerskel", MONITORING_STATISTICS_PLUGIN_INTERFACE );
	makeModule( StatisticsCollector, collector, "siox-monitoring-ThreadedStatisticsCollector", STATISTICS_COLLECTOR_INTERFACE );
	makeModule( StatisticsMultiplexer, multiplexer, "siox-monitoring-StatisticsMultiplexerSync", STATISTICS_MULTIPLEXER_INTERFACE );
	makeModule( TestListener, listener, "siox-monitoring-statisticsMultiplexerPlugin-testListener", STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE );

	#define fetchOptions(Type, ComponentVariableName) \
		Type* ComponentVariableName##Options = &ComponentVariableName->getOptions<Type>();
	fetchOptions( FileOntologyOptions, ontology );
	fetchOptions( ProviderSkeletonOptions, provider );
	fetchOptions( ThreadedStatisticsOptions, collector );
	fetchOptions( StatisticsMultiplexerSyncOptions, multiplexer );
	fetchOptions( TestListenerOptions, listener );

	ontologyOptions->filename = "optimizer-ontology-example.dat";
	providerOptions->statisticsCollector.componentPointer = collector;
	collectorOptions->ontology.componentPointer = ontology;
	collectorOptions->smux.componentPointer = multiplexer;
	listenerOptions->multiplexer.componentPointer = multiplexer;
	listenerOptions->ontology.componentPointer = ontology;

	ontology->init();
	multiplexer->init();
	collector->init();
	provider->init();
	listener->init();

	cerr << "sleeping\n";
	sleep( 1 );
	cerr << "waking up\n";

	{
		vector<shared_ptr<Statistic> > statistics = collector->availableMetrics();
		assert( statistics.size() == 3 );
		array<StatisticsValue, Statistic::kHistorySize> values[3];
		statistics[0]->getHistoricValues( HUNDRED_MILLISECONDS, &values[0], NULL );
		statistics[1]->getHistoricValues( HUNDRED_MILLISECONDS, &values[1], NULL );
		statistics[2]->getHistoricValues( HUNDRED_MILLISECONDS, &values[2], NULL );
		double expectedSum = 0;
		for(size_t i = 0; i < Statistic::kHistorySize; i++) {
			double expectedValue = 0.8*(1 << values[2][i].int32())/2;
			expectedSum += expectedValue;
			assert( values[0][i] == expectedValue );
			assert( values[1][i] == expectedValue );
		}

		StatisticsDescription description( ontology->lookup_attribute_by_name( "Statistics", "test/weather" ), {{"node", LOCAL_HOSTNAME}, {"tschaka", "test2"}} );
		array<StatisticsValue, Statistic::kHistorySize> nameLookupValues = collector->getStatistics( HUNDRED_MILLISECONDS, description );
		assert( values[1] == nameLookupValues );
		StatisticsValue aggregatedValue = collector->getReducedStatistics( SECOND, *statistics[0] );
		assert( aggregatedValue == expectedSum );

		StatisticsValue rollingValue = collector->getRollingStatistics( SECOND, *statistics[0] );
		expectedSum = 0;
		for(size_t i = 10; i--; ) expectedSum += 0.8*(1 << i);
		assert( rollingValue == expectedSum );

		assert( listener->registeredValidInput() );
	}

	delete provider;
	delete listener;
	delete collector;
	delete multiplexer;
	delete ontology;

	cerr << "OK" << endl;
	return 0;
}

