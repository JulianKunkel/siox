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
	- multiple Log() calls shall run in parallel
	- adding and removal of listener without corrupting iterator

 2 Use Cases
	- multiple plugins want use activities of a layer, the multiplexer
	  offers a facility to manage listeners and notify them on demand

 3 Design and Text
	Removing a listener might invalidate iterator
	Inserting is usually fine (for lists uneffected, vectors until resize)

	Possible solutions:
		1.	snapshot list	-> memory + time panelty

		2.	change remove to null, check when iterating, 
			clean up in additional loop, might skip if time is precious

		3.	boost::shared_lock (was actually proposend and rejected for C++11)
			would implement readers/writers 

		4. readers count + mutex

	C++11 does not provide a shared lock by it self, reasoning:
	http://permalink.gmane.org/gmane.comp.lib.boost.devel/211180


	//std::unique_lock<std::mutex> lock(mut);
	//std::lock_guard<std::mutex> lock(mut);

 4 Test Implementation
	

 */

/*! The detached threads for notify are running until the SIOX system finishes.
 When the instrumented client program finishes, a SIOX_finalize() call will be issued that sets a flag called "thrd_fin".
 */
#ifndef ACTIVITYMULTIPLEXER_IMPL1_H
#define ACTIVITYMULTIPLEXER_IMPL1_H 

#include <mutex>
#include <condition_variable>
#include <list>

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


namespace monitoring{

class ActivityMultiplexer_Impl1 : public ActivityMultiplexer
								
{
	/*
	list<MultiplexerListener<Activity> *> listeners;

	// thread safety, kept namespace verbose for clarity
	std::mutex inc;
	std::mutex dec;
	int not_invalidating = 0;



public:
	virtual void Log(Activity * element);
	virtual void registerListener(MultiplexerListener<Activity> * listener);	
	virtual void unregisterListener(MultiplexerListener<Activity> * listener);

	*/
};

}

#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
