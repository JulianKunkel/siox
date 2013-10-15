#ifndef ACTIVITY_MULTIPLEXER_PLUGIN_H
#define ACTIVITY_MULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>

using namespace core;

namespace monitoring {


	/**
	 *	A minimal interface for an ActivityMultiplexerPlugin that can be loaded
	 *	as a siox component.
	 */
	class ActivityMultiplexerPlugin: public Component {
		protected:
			ActivityMultiplexer * multiplexer;

			ActivityPluginDereferencing * facade;

			/**
			 * Custom init function that needs to be provided by plugin
			 * implementors. It is called at the end of the plugin initialisation
			 * procedure.
			 */
			virtual void initPlugin( ) = 0;

		public:
			/**
			 * ActivityMultiplexerPlugin specific init function to satisfy
			 * Core::Component
			 *
			 * It creates an multiplexer instance with the options given.
			 */
			void init() {
				ActivityMultiplexerPluginOptions & o = getOptions<ActivityMultiplexerPluginOptions>();
				assert( o.multiplexer.componentPointer != nullptr );

				multiplexer = GET_INSTANCE(ActivityMultiplexer, o.multiplexer);
				// may be NULL
				facade = GET_INSTANCE(ActivityPluginDereferencing, o.dereferenceFacade);

				initPlugin();
			}
	};

}

#define ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_activitymultiplexer_plugin"

#endif

// BUILD_TEST_INTERFACE monitoring/activity_multiplexer/plugins/ActivityMultiplexerPlugin
