#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityForwarderOptions.hpp"

using namespace monitoring;

CREATE_SERIALIZEABLE_CLS( ActivityForwarderOptions )

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityForwarder: public ActivityMultiplexerPlugin, public ActivityMultiplexerListener {
	public:
		/**
		 * Implements ActivityMultiplexerListener Notify, passes activity to out.
		 */
		virtual void Notify( Activity * element ) {
			out->Log( element );
		}

		ComponentOptions * AvailableOptions() {
			return new ActivityForwarderOptions();
		}

		void initPlugin() {
			multiplexer->registerListener( this );

			ActivityForwarderOptions & options = getOptions<ActivityForwarderOptions>();
			out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);
		}

	private:
		ActivityMultiplexer * out = nullptr;
};

extern "C"{
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
		return new ActivityForwarder();
	}
}
