#ifndef STATISTICS_PROVIDER_PLUGIN_H
#define STATISTICS_PROVIDER_PLUGIN_H

#include <string>
#include <iostream>
#include <list>
#include <boost/variant.hpp>

#include <core/component/Component.hpp>
#include <monitoring/statistics/collector/StatisticsCollector.hpp>
#include <monitoring/statistics/StatisticsTypes.hpp>
#include <monitoring/statistics/collector/StatisticsProviderPluginOptions.hpp>


namespace monitoring {

	/*
	 The StatisticsProviderPlugin gets a reference to the StatisticsCollector.
	 It registers itself in the Collector.

	 This class is used as follows:
	 The function availableMetrics() essentially yields a list of references to the last statistics values.
	 The values themselves are updated whenever nextTimestep() is called,
	 the caller can subsequently collect the updated values using the references returned by availableMetrics().
	*/
	class StatisticsProviderPlugin : public core::Component {
		private:
			StatisticsCollector * collector;
		protected:

			virtual core::ComponentOptions * AvailableOptions() {
				return new StatisticsProviderPluginOptions();
			}
		public:

			virtual StatisticsInterval minPollInterval() {
				return HUNDRED_MILLISECONDS;
			}

			///@todo TODO: This is crazy: Here we use `init()`, and `init( Options& )`, StatisticsMultiplexerPlugin uses `initPlugin()`, core::Component uses `init( Options* )`. We should really amend this by having only one virtual `init()` method, and having all `init()` methods in derived classes call the appropriate base class versions.
			virtual void init( StatisticsProviderPluginOptions & options ) {
				// default implementation is empty.
				// Override it to provide additional module-specific options.
			}

			virtual void init() {
				StatisticsProviderPluginOptions o = getOptions<StatisticsProviderPluginOptions>();

				init( o );	// call the plugin-specific method

				// now register this plugin on the collector
				collector = GET_INSTANCE( StatisticsCollector, o.statisticsCollector );

				assert(collector != nullptr);
				collector->registerPlugin( this );
			}

			virtual ~StatisticsProviderPlugin() {
				if( collector != nullptr ){
					collector->unregisterPlugin( this );
				}
			}

			/* For testing purpose the two methods are public, so you can use the plugin even without collector */
			virtual void nextTimestep() = 0;	//Update the statistics values.

			virtual vector<StatisticsProviderDatatypes> availableMetrics() = 0;	//This provides access to the statistics values.

	};

} // end namespace

#define MONITORING_STATISTICS_PLUGIN_INTERFACE "monitoring_statistics_plugin"

#endif
