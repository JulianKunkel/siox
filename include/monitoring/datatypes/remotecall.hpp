/**
 * @file    remotecall.hpp
 *
 * @description Information describing a remote call.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer
 * @date   2013
 *
 */

/**
 * USE CASES
 * =========
 * 1 - A remote call is similar to an activity, except for more detailed/complicated caller-
 * callee relations. These transcend hardware nodes and need to be tracked to allow for
 * later analysis.
 */

/**
 * REQUIREMENTS (and the use cases they are based on)
 * ============
 * 1 - Own a system-unique id.
 * 2 - Hold caller information.
 * 3 - Hold callee information.
 * 4 - Hold or be held by an activity object describing actual call.
 * 5 - Hold set (list?) of attributes used to identify call to participants?
 */

/**
 * DESIGN CONSIDERATIONS
 * =====================
 *
 */

/**
 * OPEN ISSUES
 * ===========
 */

#ifndef __SIOX_REMOTECALL_H
#define __SIOX_REMOTECALL_H


namespace monitoring {

using std::string;


/**
 *
 */
struct RemoteCall
{
  RemoteCall(void);
};

}

#endif // __SIOX_REMOTECALL_H