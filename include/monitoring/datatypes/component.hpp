/**
 * @file    component.hpp
 *
 * @description A (software) component.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer
 * @date   2013
 *
 */

/**
 * USE CASES
 * =========
 * 1 - Encapsulates a (software) component, as hardware components without some kind of software
 * running on it (such as an API or a driver) make no sense for us.
 * Objects represent the component as archived via the monitoring path (and with global IDs),
 * not as used for working node-locally.
 */

/**
 * REQUIREMENTS (and the use cases they are based on)
 * ============
 * 1 - Own a globally unique component id (CId).
 * 2 - Hold the software Layer/API represented ("POSIX" or "MPI") as an id.
 * 3 - Hold the software version and/or build ("OpenMP" and/or "1.12.23b") as an id.
 * 4 - Hold the hardware the component runs on ("node42" or "SeagateMX756") as an id.
 * 5 - Hold the instance the component represents ( MPI rank, TCP port, ...).
 * 6 - Hold the component's runtime environment (see class RuntimeInfo).
 * 7 - Hold a set (list?) of additional attributes describing the component
 * (capacity, cache size,...)
 * 8 - Once set, all members are immutable.
 */

/**
 * DESIGN CONSIDERATIONS
 * =====================
 * 1 - The object is initialised upon creation by siox-ll's component_register().
 * 2 - To speed up processing, the CID may only be assigned by the node-local daemon
 * once the first activity from this component is 
 *
 */

/**
 * OPEN ISSUES
 * ===========
 * 1 - Should the instance info be stored as an ID?
 * 2 - Possibly, the component could hold an enum indicating its status.
 */

#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H


#include <attribute.h>
#include <runtimeinfo.h>


// using std::string;


namespace monitoring
{


class Component
{
private:
    uint64_t cid;
    uint64_t hwid;
    uint64_t swid;
    uint64_t iid;
    // List<Attribute> attributes;
    RuntimeInfo runtime;

public:
  Component(void);
  // Component(const uint64_t &Cid, const uint64_t &HWid, const uint64_t &SWid, const uint64_t &Iid) : cid (Cid), hwid (HWid), swid (SWid), iid (Iid) {};
  ~Component(void);

  uint64_t get_CId() const {return cid; };
  uint64_t get_SwId() const {return swid; };
  uint64_t get_HwId() const {return hwid; };
  uint64_t get_IId() const {return iid; };
};

}

#endif // SIOX_COMPONENT_H
