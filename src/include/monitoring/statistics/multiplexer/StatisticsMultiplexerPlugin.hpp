#ifndef STATISTICSMULTIPLEXER_PLUGIN_H
#define STATISTICSMULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerListener.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPluginOptions.hpp>

namespace monitoring {

	class StatisticsMultiplexerPlugin: public core::Component, public StatisticsMultiplexerListener {
		protected:

			virtual void initPlugin() throw() = 0;

		public:

			void init() override{
				StatisticsMultiplexerPluginOptions* o = &getOptions<StatisticsMultiplexerPluginOptions>();
				assert( o && "either no options were set, or they are of the wrong type" );

				StatisticsMultiplexer* multiplexer = GET_INSTANCE( StatisticsMultiplexer, o->multiplexer );
				assert( multiplexer );

				initPlugin();
				multiplexer->registerListener( this );
			}

			void finalize() override{
				StatisticsMultiplexerPluginOptions* o = &getOptions<StatisticsMultiplexerPluginOptions>();
				StatisticsMultiplexer* multiplexer = GET_INSTANCE( StatisticsMultiplexer, o->multiplexer );
				multiplexer->unregisterListener( this );
			}
	};

}

#define STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_statisticsmultiplexer_plugin"

#endif

