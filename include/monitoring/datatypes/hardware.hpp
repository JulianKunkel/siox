/**
 * @file    hardware.hpp
 *
 * @description A class describing a hardware entity.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer
 * @date   2013
 *
 */

/**
 * USE CASES
 * =========
 * 1 - Hold a hardware entity's data.
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

#ifndef SIOX_HARDWARE_H
#define SIOX_HARDWARE_H

namespace monitoring
{


class Hardware
{
    long hwid;
    char[] description;

public:
    Hardware(char[] description);
};


}

#endif // SIOX_HARDWARE_H