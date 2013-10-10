#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityForwarderOptions.hpp"

using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityForwarder: public ActivityMultiplexerPlugin, public ActivityMultiplexerListener {
	public:
		/**
		 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
		 */
		virtual void Notify( Activity * element ) {
			out->Log( element );
		}

		/**
		 * Dummy implementation for ActivityForwarder Options.
		 */
		ComponentOptions * AvailableOptions() {
			return new ActivityForwarderOptions();
		}

		/**
		 * Forwarder setup:
		 * Register this to a already created multiplexer
		 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
		 * Create a second ActivityMultiplexer instance that serves as an
		 * out going end of the Forwarder.
		 */
		void initPlugin() {
			multiplexer->registerListener( this );

			ActivityForwarderOptions & options = getOptions<ActivityForwarderOptions>();
			out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);
		}

	private:
		/** Receiving ActivityMultiplexer */
		ActivityMultiplexer * out = nullptr;
};

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityForwarder();
	}
}
