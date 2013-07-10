#ifndef ACTIVITYMULTIPLEXER_H
#define ACTIVITYMULTIPLEXER_H 

#include <monitoring/datatypes/Activity.hpp>
//#include <monitoring/multiplexer/Multiplexer.hpp>
#include <template/monitoring/multiplexer/Multiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <core/component/Component.hpp>

namespace monitoring{


/**
 * ActivityMultiplexer
 * Forwards logged activities to registered listeners (e.g. Plugins) either
 * in an syncronised or asyncronous manner.
 */
//class ActivityMultiplexer : public core::Component
class ActivityMultiplexer : public MultiplexerTemplate<Activity>
{
//public:
//	virtual void Log(Activity * element) = 0;
//
//	/**
//	 * Register listener to multiplexer
//	 *
//	 * @param	MultiplexerListener *	listener	listener to notify in the future
//	 */
//	virtual void registerListener(ActivityMultiplexerListener * listener) = 0;

//	/**
//	 * Unregister listener from multiplexer
//	 *
//	 * @param	MultiplexerListener *	listener	listener to remove
//	 */
//	virtual void unregisterListener(ActivityMultiplexerListener * listener) = 0;
	
};


#define ACTIVITY_MULTIPLEXER_INTERFACE "monitoring_activitymultiplexer"

}
#endif /* ACTIVITYMULTIPLEXER_H */
