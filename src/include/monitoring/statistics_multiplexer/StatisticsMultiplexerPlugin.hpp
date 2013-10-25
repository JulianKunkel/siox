#ifndef STATISTICSMULTIPLEXER_PLUGIN_H
#define STATISTICSMULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerListener.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerPluginOptions.hpp>
#include <monitoring/statistics_multiplexer/StatisticsPluginDereferencing.hpp>

using namespace core;

namespace monitoring {

	class StatisticsMultiplexerPlugin: public Component, public StatisticsMultiplexerListener {
		protected:

			virtual void init( StatisticsMultiplexerPluginOptions * options, StatisticsMultiplexer & multiplexer ) = 0;

		public:
			void init( StatisticsMultiplexerPluginOptions * options, StatisticsMultiplexer * statistics_multiplexer ) {

				init( options );
			}

			void init( ComponentOptions * options ) {
				StatisticsMultiplexerPluginOptions * o = ( StatisticsMultiplexerPluginOptions * ) options;
				assert( options != nullptr );
				assert( o->multiplexer.componentID != 0 );

				init( o, o->multiplexer.instance<StatisticsMultiplexer>(), o->dereferenceFacade.instance<StatisticsPluginDereferencing>() );
			}
	};

}

#define STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_statisticsmultiplexer_plugin"

#endif

