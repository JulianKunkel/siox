#include <iostream>

#include <core/module/ModuleLoader.hpp>
#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>

#include <monitoring/statistics_collector/StatisticsCollector.hpp>

#include "../ProviderSkeletonOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

class StatisticsCollectorTester : public StatisticsCollector {
	public:
		virtual void registerPlugin( StatisticsProviderPlugin * plugin ) {

			auto list = plugin->availableMetrics();

			cout << "new plugin" << endl;

			for( int i = 0; i < 5; i++ ) {
				for( auto it = list.begin() ; it != list.end(); it ++ ) {
					StatisticsProviderDatatypes & stat = *it;
					cout << i << " " << stat.metrics << ": " << stat.value << endl;
				}
				plugin->nextTimestep();
			}
		}

		virtual void unregisterPlugin( StatisticsProviderPlugin * plugin ) {

		}

		virtual vector<shared_ptr<Statistic> > getStatistics() throw() {
			return vector<shared_ptr<Statistic> >();
		}
		virtual array<StatisticsValue, Statistic::kHistorySize> getStatistics( StatisticsInterval interval, const StatisticsDescription & stat ) throw() {
			return array<StatisticsValue, Statistic::kHistorySize>();
		}

//		virtual StatisticsValue getRollingStatistics( StatisticsInterval interval, StatisticsDescription & stat ) {
//			return StatisticsValue( 1 );
//		}
//
//		virtual StatisticsValue getReducedStatistics( StatisticsInterval interval, StatisticsDescription & stat, StatisticsReduceOperator op ) {
//			return StatisticsValue( 1 );
//		}
//
//		virtual StatisticsValue getStatistics( StatisticsInterval interval, StatisticsDescription & stat, StatisticsReduceOperator op ) {
//			return StatisticsValue( 1 );
//		}
//
//		virtual vector<StatisticsDescription> availableMetrics() {
//			vector<StatisticsDescription> metrics;
//			return metrics;
//		}


		virtual void init() {

		}

		virtual ComponentOptions * AvailableOptions() {
			return new ComponentOptions();
		}
};

int main( int argc, char const * argv[] )
{
	StatisticsProviderPlugin * ps = module_create_instance<StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-providerskel" , MONITORING_STATISTICS_PLUGIN_INTERFACE );

	ps->getOptions<ProviderSkeletonOptions>().statisticsCollector.componentID = ( ComponentReferenceID ) new StatisticsCollectorTester();

	ps->init();

	delete( ps );

	cout << "OK" << endl;
	return 0;
}


