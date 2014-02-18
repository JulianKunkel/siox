#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityForwarderOptions.hpp"

using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityForwarder: public ActivityMultiplexerPlugin {
	public:
		/**
		 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
		 */
		void Notify( const shared_ptr<Activity> & element, int lostCount ) {
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
			ActivityForwarderOptions & options = getOptions<ActivityForwarderOptions>();
			out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);
			multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &ActivityForwarder::Notify ), false );
		}

		void finalize() {
			multiplexer->unregisterCatchall( this, false );
			ActivityMultiplexerPlugin::finalize();
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
