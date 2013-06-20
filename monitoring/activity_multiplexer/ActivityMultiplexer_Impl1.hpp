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
 */

/*! The detached threads for notify are running until the SIOX system finishes.
 When the instrumented client program finishes, a SIOX_finalize() call will be issued that sets a flag called "thrd_fin".
 */
// that flag is global?
#ifndef ACTIVITYMULTIPLEXER_IMPL1_H
#define ACTIVITYMULTIPLEXER_IMPL1_H 

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

namespace monitoring{

class ActivityMultiplexer_Impl1
{


};

}

#endif /* ACTIVITYMULTIPLEXER_IMPL1_H */
