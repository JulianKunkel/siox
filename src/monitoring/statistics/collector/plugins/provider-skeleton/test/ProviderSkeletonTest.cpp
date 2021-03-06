#include <iostream>

#include <core/module/ModuleLoader.hpp>
#include <monitoring/statistics/collector/StatisticsProviderPlugin.hpp>

#include <monitoring/statistics/collector/StatisticsCollector.hpp>

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

		virtual vector<shared_ptr<Statistic> > availableMetrics() throw() {
			return vector<shared_ptr<Statistic> >();
		}

		virtual std::shared_ptr<Statistic> getStatistic( const std::string& ontologyAttribute, const std::string& topologyPath ) throw() { return std::shared_ptr<Statistic>(); }

		virtual void registerCollection( StatisticsCollection* collection ) throw() {}

		virtual void unregisterCollection( StatisticsCollection* collection ) throw() {}

		virtual array<StatisticsValue, Statistic::kHistorySize> getStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw() {
			return array<StatisticsValue, Statistic::kHistorySize>();
		}

		virtual StatisticsValue getRollingStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw() {
			return StatisticsValue( 1 );
		}

		virtual StatisticsValue getReducedStatistics( StatisticsReduceOperator reductionOp, StatisticsInterval interval, const StatisticsDescription & stat ) throw() {
			return StatisticsValue( 1 );
		}


		virtual void init() {

		}

		virtual ComponentOptions * AvailableOptions() {
			return new ComponentOptions();
		}
};

int main( int argc, char const * argv[] )
{
	StatisticsProviderPlugin * ps = module_create_instance<StatisticsProviderPlugin>( "", "siox-monitoring-statisticsPlugin-providerskel" , MONITORING_STATISTICS_PLUGIN_INTERFACE );

	ps->getOptions<ProviderSkeletonOptions>().statisticsCollector.componentPointer = new StatisticsCollectorTester();

	StatisticsProviderPluginOptions options;
	ps->init(& options);

	delete( ps );

	cout << "OK" << endl;
	return 0;
}


