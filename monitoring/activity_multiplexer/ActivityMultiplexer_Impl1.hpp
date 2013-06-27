/*!
 * @file    ActivityMultiplexer_Impl1.hpp
 *
 * @description 
 * As a follow up to the SIOX system history this is the activity multiplexer 
 * for concurrent activity handling.
 * 
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

	- 

 2 Use Cases
	- multiple plugins want use activities of a layer, the multiplexer
	  offers a facility to manage listeners and notify them on demand

 3 Design and Text
	plain Listener/Observer martured, see Multiplexer.hpp for details

	Solutions:
	1.	snapshot list -> memory + time panelty


 4 Test Implementation
	

 */

/*! The detached threads for notify are running until the SIOX system finishes.
 When the instrumented client program finishes, a SIOX_finalize() call will be 
 issued that sets a flag called "thrd_fin".
 */
#ifndef ACTIVITYMULTIPLEXER_IMPL1_H
#define ACTIVITYMULTIPLEXER_IMPL1_H 

#include <iostream>

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerAsync.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


namespace monitoring{

class ActivityMultiplexer_Impl1 : ActivityMultiplexerAsync
{

};


}

#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
