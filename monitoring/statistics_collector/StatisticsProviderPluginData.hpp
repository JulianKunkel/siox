/*
 * This class bundles all the information specific to each plugin registered with a statistics collector.
 *
 * Rationale: Why is the polling on a per-plugin basis?
 * Each plugin may have a different polling frequency. This is much easier to handle with this plugin wrapper.
 * Plugins are registered/unregistered while the polling thread is running, so a central table of data is difficult to maintain.
 *
 * Polling is assumed to be initiated at the maximum frequency at all times. If the plugin is configured to use a lower polling frequency, the StatisticsProvidePluginData object will ignore polling requests accordingly.
 *
 * It is assumed that there is only one thread that invokes doPolling() on an object, so that almost no synchronization has to be done.
 * The user only has to ensure that no thread ever sees an incompletely constructed or destructed object.
 */

#ifndef INCLUDE_GUARD_MONITORING_STATISTICS_PROVIDER_PLUGIN_DATA
#define INCLUDE_GUARD_MONITORING_STATISTICS_PROVIDER_PLUGIN_DATA

#include <stdlib.h>
#include <vector>

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <monitoring/statistics_collector/StatisticsProviderPlugin.hpp>
#include <monitoring/statistics_collector/StatisticsProviderDatatypes.hpp>

namespace monitoring {

	class StatisticsProviderPluginData {
		public:
			const static size_t kHistorySize = 10;

			StatisticsProviderPluginData( StatisticsProviderPlugin* plugin ) throw();

			const std::vector<StatisticsProviderDatatypes>& availableMetrics() const throw();
			StatisticsProviderPlugin* getPlugin() const throw();
			StatisticsValue* getValue( StatisticsInterval interval, size_t statisticIndex, ssize_t measurementIndex ) throw();
			StatisticsValue* getLastValue( StatisticsInterval interval, size_t statisticIndex ) throw();
			StatisticsValue* getLine( StatisticsInterval interval, size_t age) throw();	//getLine(..., 0) produces a pointer to the latest values in the given interval class, getLine(..., 1) goes back by the indicated interval. age must be smaller than kHistorySize.
			void doPolling() throw();	//asks the plugin for new data, updating the higher statistics as necessary.

			~StatisticsProviderPluginData() throw();

		private:
			static size_t measurementIncrement( StatisticsInterval pollInterval ) throw();

			StatisticsProviderPlugin* plugin;
			StatisticsInterval pollInterval;
			size_t statisticsCount;
			ssize_t lastMeasurementIndex;
			std::vector<StatisticsProviderDatatypes> sourceDescriptions;
			StatisticsValue* values;

			//Stuff we don't want to have
			StatisticsProviderPluginData() = delete;
			StatisticsProviderPluginData(const StatisticsProviderPluginData&) = delete;
			StatisticsProviderPluginData& operator=(const StatisticsProviderPluginData&) = delete;
	};



	inline size_t StatisticsProviderPluginData::measurementIncrement( StatisticsInterval pollInterval ) throw() {
		static_assert(kHistorySize >= 10, "kHistorySize is not big enough to aggregate the statistics from one interval level to the next");
		switch(pollInterval) {
			case HUNDRED_MILLISECONDS: return 1;
			case SECOND: return 10;
			case TEN_SECONDS: return 100;
			case MINUTE: return 600;
			case TEN_MINUTES: return 6000;
			default: assert(0), abort();
		}
	}

	inline StatisticsProviderPlugin* StatisticsProviderPluginData::getPlugin() const throw() {
		return plugin;
	}

	inline StatisticsValue* StatisticsProviderPluginData::getValue( StatisticsInterval interval, size_t statisticIndex, ssize_t measurementIndex ) throw() {
		return &values[(interval*(kHistorySize + 1) + measurementIndex%(kHistorySize + 1))*statisticsCount + statisticIndex];
	}

	inline StatisticsValue* StatisticsProviderPluginData::getLastValue( StatisticsInterval interval, size_t statisticIndex ) throw() {
		return getValue( interval, statisticIndex, lastMeasurementIndex / measurementIncrement( interval ) );
	}

	inline StatisticsValue* StatisticsProviderPluginData::getLine( StatisticsInterval interval, size_t age ) throw() {
		return getValue( interval, 0, lastMeasurementIndex/measurementIncrement( interval ) - age );
	}

}

#endif
