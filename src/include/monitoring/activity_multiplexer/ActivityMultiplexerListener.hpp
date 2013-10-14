#ifndef ACTIVITY_MULTIPLEXER_LISTENER_H
#define ACTIVITY_MULTIPLEXER_LISTENER_H

#include <monitoring/datatypes/Activity.hpp>

namespace monitoring {
	/**
	 * ActivityMultiplexerListener
	 * Interface plugins that want to register to a multiplexer need to implement.
	 */

	class ActivityMultiplexerListener {
		public:
			/**
			 * Receive logged activity from a multiplexer
			 *
			 * @param    activity	a logged Activity
			 */
			virtual void Notify( Activity * activity ) = 0;
	};


	class ActivityMultiplexerListenerAsync {
		public:
			/*
			 * lost activities counts the of activities which have been lost before processing the activity.
			 *
			 * @param	lostActivitiesCount	numbers of activities lost before this activity came through
			 * @param	activtiy	a logged activity
			 */
			virtual void NotifyAsync( int lostActivitiesCount, Activity * activity ) = 0;
	};

}

#endif /* ACTIVITY_MULTIPLEXER_LISTENER_H */
