/**
 * @file    software.hpp
 *
 * @description A class describing a software entity.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer
 * @date   2013
 *
 */

/**
 * USE CASES
 * =========
 */

/**
 * REQUIREMENTS (and the use cases they are based on)
 * ============
 * 1 - Own a system-unique id.
 * 2 - Hold a field for comments and human-readable descriptions.
 * 3 - Hold a set (list?) of attributes?
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

#ifndef SIOX_SOFTWARE_H
#define SIOX_SOFTWARE_H

namespace monitoring
{


class Software
{
    long swid;
    char[] description;

public:
    Software(char[] description);
};


}

#endif // SIOX_SOFTWARE_H

