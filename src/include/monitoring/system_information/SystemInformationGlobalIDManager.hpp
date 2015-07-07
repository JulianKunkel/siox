#ifndef __SYSTEMINFO_H
#define __SYSTEMINFO_H

#include <string>

#include <core/component/Component.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

//#include <monitoring/system_information/SysteminfoDatatypes.hpp>

using namespace std;

namespace monitoring {

	class SystemInformationGlobalIDManager : public core::Component {
		public:

			virtual NodeID                      register_nodeID( const string & hostname ) = 0;
			virtual NodeID                      lookup_nodeID( const string & hostname ) const throw( NotFoundError ) = 0;
			virtual const string                lookup_node_hostname( NodeID id ) const throw( NotFoundError )  = 0;


			virtual DeviceID                    register_deviceID( NodeID id, const  string & local_unique_identifier ) = 0;
			virtual DeviceID                    lookup_deviceID( NodeID id, const  string & local_unique_identifier ) const throw( NotFoundError ) = 0;
			virtual const string                lookup_device_local_name( DeviceID id ) const throw( NotFoundError ) = 0;
			virtual NodeID                      lookup_node_of_device( DeviceID id ) const throw( NotFoundError ) = 0;

			virtual FilesystemID                register_filesystemID( const string & global_unique_identifier ) = 0;
			virtual FilesystemID                lookup_filesystemID( const string & global_unique_identifier ) const throw( NotFoundError ) = 0;
			virtual const string                lookup_filesystem_name( FilesystemID id ) const throw( NotFoundError )  = 0;

			virtual UniqueInterfaceID           register_interfaceID( const string & interface, const string & implementation )  = 0;
			virtual UniqueInterfaceID           lookup_interfaceID( const string & interface, const string & implementation ) const throw( NotFoundError )  = 0;
			virtual const string                lookup_interface_name( UniqueInterfaceID id ) const throw( NotFoundError )  = 0;
			virtual const string                lookup_interface_implementation( UniqueInterfaceID id ) const throw( NotFoundError ) = 0;

			virtual UniqueComponentActivityID   register_activityID( UniqueInterfaceID id, const string & name )  = 0;
			virtual UniqueComponentActivityID   lookup_activityID( UniqueInterfaceID id, const string & name ) const throw( NotFoundError )  = 0;
			virtual UniqueInterfaceID           lookup_interface_of_activity( UniqueComponentActivityID id ) const throw( NotFoundError )  = 0;
			virtual const string                lookup_activity_name( UniqueComponentActivityID id ) const throw( NotFoundError )  = 0;
	
	};

}

#define SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE "monitoring_systeminformation_global_id"


#endif

// BUILD_TEST_INTERFACE monitoring/systeminformation/modules/systeminformationIDManager
