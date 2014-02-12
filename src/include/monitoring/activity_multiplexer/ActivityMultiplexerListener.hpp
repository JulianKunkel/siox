#ifndef ACTIVITY_MULTIPLEXER_LISTENER_H
#define ACTIVITY_MULTIPLEXER_LISTENER_H

#include <monitoring/datatypes/Activity.hpp>

namespace monitoring {
	/**
	 * ActivityMultiplexerListener
	 * Interface plugins that want to register to a multiplexer need to implement.
	 */

	class ActivityMultiplexerListener {
		//Just an empty interface because, instead of registering listeners with a predefined interface, we are now registering listener - member function pointer combinations with the multiplexer. Thus, we still need a common type for the listeners, but there are no fixed function names anymore.
	};
}

#endif /* ACTIVITY_MULTIPLEXER_LISTENER_H */
