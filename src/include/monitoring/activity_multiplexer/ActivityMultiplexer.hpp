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
	 *
	 * \dotfile pictures/multiplexer.dot Multiplexer
	 *
	 */
	class ActivityMultiplexer : public core::Component {
		public:
			//The same callback type is used for synchronous and asynchronous handlers to allow a listener to install the same handler both for a synchronous and an asynchronous callback. This might even be configurable via the siox.conf. Also, it avoids unnecessary code duplication. If the callback is registered synchronously, it will just always get a `lostActivityCount` of zero.
			typedef void ( ActivityMultiplexerListener::*Callback )( const shared_ptr<Activity>& activity, int lostActivitiesCount);

			/**
			 * Notify ActivityMultiplexer that an activity needs to be passed to listerns
			 *
			 * @param	activity	logged activity
			 */
			virtual void Log( const shared_ptr<Activity> & activity ) = 0;

			virtual void registerForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, Callback handler, bool async ) = 0;
			virtual void unregisterForUcaid( UniqueComponentActivityID ucaid, ActivityMultiplexerListener* listener, bool async ) = 0;
			virtual void registerCatchall( ActivityMultiplexerListener* listener, Callback handler, bool async ) = 0;
			virtual void unregisterCatchall( ActivityMultiplexerListener* listener, bool async ) = 0;
	};

	// An ActivityMultiplexer which is called before a call completed, thus it may be incompleted.
	class ActivityMultiplexerSync : public ActivityMultiplexer{

	};


#define ACTIVITY_MULTIPLEXER_INTERFACE "monitoring_activitymultiplexer"

}
#endif /* ACTIVITYMULTIPLEXER_H */
