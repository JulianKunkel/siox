#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityMutatorOptions.hpp"

#include <iostream>
#include <string>


using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityMutator: public ActivityMultiplexerPlugin {
	public:
		/**
		 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
		 */
		void Notify( const shared_ptr<Activity> & element, int lostCount ) {

			
			std::cout << "ActivityMutator was here!" << std::endl;;

			out->Log( element );
		}

		/**
		 * Dummy implementation for ActivityMutator Options.
		 */
		ComponentOptions * AvailableOptions() {
			return new ActivityMutatorOptions();
		}

		/**
		 * Mutator setup:
		 * Register this to a already created multiplexer
		 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
		 * Create a second ActivityMultiplexer instance that serves as an
		 * out going end of the Mutator.
		 */
		void initPlugin() {
			ActivityMutatorOptions & options = getOptions<ActivityMutatorOptions>();
			out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);
			multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &ActivityMutator::Notify ), false );
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
		return new ActivityMutator();
	}
}
