#ifndef __SYSTEMINFO_H
#define __SYSTEMINFO_H

#include <string>

#include <core/component/Component.hpp>

#include <monitoring/system_information/SysteminfoDatatypes.hpp>

using namespace std;

namespace monitoring{

namespace system_information{

class SystemInformationGlobalIDManager : public core::Component{
public:

	virtual NodeID						node_by_id(string & hostname) = 0;
	virtual DeviceID					device_by_id(NodeID hw, string & local_unique_identifier) = 0;
	virtual FilesystemID				filesystem_by_id(string & global_unique_identifier) = 0;

    virtual const string &              node_hostname(NodeID id) = 0;
    virtual const string &              device_local_name(DeviceID id) = 0;
    virtual const string &              filesystem_name(FilesystemID id) = 0;


	virtual UniqueInterfaceID           interface_by_id(string & interface, string & implementation)  = 0;
    virtual UniqueComponentActivityID   activity_by_id(UniqueInterfaceID & id, string & name)  = 0;

    virtual const string & 				interface_name(UniqueInterfaceID id)  = 0;
    virtual const string & 				interface_implementation(UniqueInterfaceID id) = 0;
    virtual const string & 				activity_name(UniqueComponentActivityID id) = 0;
};

}
}

#endif