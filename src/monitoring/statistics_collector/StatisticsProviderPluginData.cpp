#include <atomic>

#include "StatisticsProviderPluginData.hpp"

namespace monitoring {

	StatisticsProviderPluginData::StatisticsProviderPluginData( StatisticsProviderPlugin* plugin ) throw() :
		plugin(plugin),
		pollInterval(plugin->minPollInterval())
	{
		// TODO use the options as set by the user.
		// the configInterval must be at least minPollInterval().
		sourceDescriptions = plugin->availableMetrics();
		statisticsCount = sourceDescriptions.size();
		lastMeasurementIndex = -measurementIncrement(pollInterval);
		values = new StatisticsValue[statisticsCount*(kHistorySize + 1)*INTERVALLS_NUMBER];
	}

	const vector<StatisticsProviderDatatypes>& StatisticsProviderPluginData::availableMetrics() const throw() {
		return sourceDescriptions;
	}


	void StatisticsProviderPluginData::doPolling() throw() {
		size_t newMeasurementIndex = lastMeasurementIndex + 1;
		if( !( newMeasurementIndex % measurementIncrement( pollInterval ) ) ) {
			printf("Polled.\n");
			StatisticsValue* curLine = getLine( pollInterval, kHistorySize );
			plugin->nextTimestep();
			for( size_t i = statisticsCount; i--; ) {
				curLine[i] = sourceDescriptions[i].value;
			}
			//TODO: Aggregate the statistics from the polling interval level into the higher interval levels.
		}
		std::atomic_thread_fence( std::memory_order_release );
		lastMeasurementIndex = newMeasurementIndex;
	}

	StatisticsProviderPluginData::~StatisticsProviderPluginData() throw() {
		delete[] values;
	}

}
