#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring {


	/**
	 * ActivityMultiplexer
	 * Forwards logged activities to registered listeners (e.g. Plugins) either
	 * in an syncronised or asyncronous manner.
	 */
	class ActivityMultiplexer : public core::Component {
		public:
			virtual void Log( Activity * activity ) = 0;

			/**
			 * Register listener to multiplexer
			 *
			 * @param   listener    listener to notify in the future
			 */
			virtual void registerListener( ActivityMultiplexerListener * listener ) = 0;


			/**
			 * Unregister listener from multiplexer
			 *
			 * @param   listener    listener to remove
			 */
			virtual void unregisterListener( ActivityMultiplexerListener * listener ) = 0;

			/**
			 * Register async listener to multiplexer
			 *
			 * @param   listener    listener to notify in the future
			 */
			virtual void registerAsyncListener( ActivityMultiplexerListenerAsync * listener ) = 0;


			/**
			 * Unregister async listener from multiplexer
			 *
			 * @param   listener    listener to remove
			 */
			virtual void unregisterAsyncListener( ActivityMultiplexerListenerAsync * listener ) = 0;
	};


#define ACTIVITY_MULTIPLEXER_INTERFACE "monitoring_activitymultiplexer"

}
#endif /* ACTIVITYMULTIPLEXER_H */
