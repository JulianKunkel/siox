/**
 * @date 2014
 * @author Nathanael Hübbe
 */

#ifndef INCLUDE_GUARD_SRC_MONITORING_STATISTICS_COLLECTOR_PLUGINS_QUALITATIVE_UTILIZATION_STATISTICS_INTEGRATOR
#define INCLUDE_GUARD_SRC_MONITORING_STATISTICS_COLLECTOR_PLUGINS_QUALITATIVE_UTILIZATION_STATISTICS_INTEGRATOR

#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerListener.hpp>
#include <monitoring/statistics/StatisticsIntegratorOptions.hpp>

namespace monitoring {
	class StatisticsIntegrator : public StatisticsProviderPlugin, public StatisticsMultiplexerListener {
		public:
			virtual void initPlugin() throw() {}

			void init() {
				StatisticsProviderPlugin::init();
			}

			void init( StatisticsProviderPluginOptions * options ) override {
				StatisticsIntegratorOptions* o = dynamic_cast<StatisticsIntegratorOptions*>(options);
				assert( o && "either no options were set, or they are of the wrong type" );

				StatisticsMultiplexer* multiplexer = GET_INSTANCE( StatisticsMultiplexer, o->multiplexer );
				assert( multiplexer );

				initPlugin();
				multiplexer->registerListener( this );
			}

			void finalize() override {
				StatisticsIntegratorOptions* o = &getOptions<StatisticsIntegratorOptions>();
				StatisticsMultiplexer* multiplexer = GET_INSTANCE( StatisticsMultiplexer, o->multiplexer );
				multiplexer->unregisterListener( this );
			}
	};
}

#endif
