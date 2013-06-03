/**
 * @file    runtimeinfo.hpp
 *
 * @description An entity's runtime information.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer
 * @date   2013
 *
 */

/**
 * USE CASES
 * =========
 * 1 - A component needs to store the information dependent on its current invocation.
 */

/**
 * REQUIREMENTS (and the use cases they are based on)
 * ============
 * 1 - Hold version or build info of a component.
 * 2 - Hold the process id of a component.
 * 3 - Hold the parent process id of a component?
 * 4 - Hold the thread id of a component.
 * 5 - Hold the command line initiating the original top-level call.
 * 6 - Once set, all members are to be immutable.
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

#ifndef __SIOX_RUNTIMEINFO_H
#define __SIOX_RUNTIMEINFO_H


namespace monitoring {

using std::string;


/**
 * An entity's runtime information
 *
 * Collects all information that is common to all components but different for each.
 */
struct RuntimeInfo
{
  long  pid;  // Process id
  long  tid;  // Thread id
  string  commandline;  // Command line used to call original application

public:
  // Base constructor setting up an empty object.
  RuntimeInfo(void);
  // Full constructor; default parameters will result in SIOX looking up the info on its own.
  RuntimeInfo(string version, long pid = 0, long tid = 0, string commandline = "");
};

}

#endif // __SIOX_RUNTIMEINFO_H