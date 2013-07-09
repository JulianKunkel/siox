#ifndef __SYSTEMINFO_H
#define __SYSTEMINFO_H

#include <string>

#include <core/component/Component.hpp>

#include <monitoring/system_information/SysteminfoDatatypes.hpp>

using namespace std;

namespace monitoring{

class SystemInformationGlobalIDManager : public core::Component{
public:

	virtual NodeID						node_id(const string & hostname) = 0;
	virtual DeviceID					device_id(NodeID id, const  string & local_unique_identifier) = 0;
	virtual FilesystemID				filesystem_id(const string & global_unique_identifier) = 0;

    virtual string &              node_hostname(NodeID id) = 0;
    virtual string &              device_local_name(DeviceID id) = 0;
    virtual string &              filesystem_name(FilesystemID id) = 0;


	virtual UniqueInterfaceID           interface_id(const string & interface, const string & implementation)  = 0;
    virtual UniqueComponentActivityID   activity_id(UniqueInterfaceID & id, const string & name)  = 0;

    virtual string & 				interface_name(UniqueInterfaceID id)  = 0;
    virtual string & 				interface_implementation(UniqueInterfaceID id) = 0;
    virtual string & 				activity_name(UniqueComponentActivityID id) = 0;
};

}

#define SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE "monitoring_systeminformation_global_id"


#endif

// BUILD_TEST_INTERFACE monitoring/systeminformation/modules/systeminformationIDManager
