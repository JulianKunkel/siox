/*!
 * @file    ActivityMultiplexer_Impl1.hpp
 *
 * @description As a follow up to the SIOX system history this is the activity multiplexer for concurrent activity handling.
 * @standard    Preferred standard is C++11
 *
 * @author Jakob Luettgau, Julian Kunkel
 * @date   2013
 *
 */

/*!
 Software Structure
 1 Requirements



 2 UseCases
 3 Design und Text
 4 Testimplementation
 
 5 Considerations
	Removing a listener might invalidate iterator
	Inserting is usually fine (for lists uneffected, vectors until resize)

	Solutions:
	1.	snapshot list	-> memory + time panelty

	2.	change remove to null, check when iterating, 
		clean up in additional loop, might skip if time is precious

 */

/*! The detached threads for notify are running until the SIOX system finishes.
 When the instrumented client program finishes, a SIOX_finalize() call will be issued that sets a flag called "thrd_fin".
 */
#ifndef ACTIVITYMULTIPLEXER_IMPL1_H
#define ACTIVITYMULTIPLEXER_IMPL1_H 

#include <list>

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

namespace monitoring{

class ActivityMultiplexer_Impl1 : public ActivityMultiplexer
{
public:
	virtual void Log(Activity * element);
	virtual void registerListener(MultiplexerListener<Activity> * listener);	
	virtual void unregisterListener(MultiplexerListener<Activity> * listener);

private:
	list<MultiplexerListener<Activity> *> listeners;
};

}

#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
