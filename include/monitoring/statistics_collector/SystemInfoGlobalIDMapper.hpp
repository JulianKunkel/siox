#ifndef __SYSTEMINFO_GLOBALID_MAPPER_H
#define __SYSTEMINFO_GLOBALID_MAPPER_H

// Primary Focus: The Mapping of hostname to hostID

// in monitoring/system_information/SystemInformationGlobalIDManager.hpp global ids such as NodeID which is our hostID are defined and made available with the interface SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE

// This SIG mapper uses the Hostid from the SIG manager for mapping the hostname to a certain hostID

#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

class SystemInformationGlobalIDMapper : public core::Component{
public:

	/* These functions are used to create the HostID */
	virtual NodeID create_host_mapping(const string & value) = 0;
	virtual const string * lookup_host_mapping(HostID id) = 0;

}

#endif
