#include <assert.h>
#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

#include <monitoring/system_information/modules/TopologySystemInformation/TopologySystemInformationOptions.hpp>
#include <monitoring/topology/Topology.hpp>

using namespace std;

using namespace monitoring;
using namespace core;

int main( int argc, char const * argv[] )
{
	SystemInformationGlobalIDManager * s = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-TopologySystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );

	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );	
	topology->init();


	// init plugin
	// not necessary, but for testing...
	TopologySystemInformationOptions * op = new TopologySystemInformationOptions();
	op->topology.componentPointer = topology ;
	s->init( op );


	UniqueInterfaceID uid = s->register_interfaceID( "MPI", "MPICH2" );
	assert( is_valid_id( uid ) );
	string inref = s->lookup_interface_name( uid );
	assert( inref == "MPI" );
	string interface_implementation = s->lookup_interface_implementation( uid );
	assert( interface_implementation == "MPICH2" );

	UniqueInterfaceID uid2 = s->register_interfaceID( "MPI", "OpenMPI" );
	assert( is_valid_id( uid2 ) );
	inref = s->lookup_interface_name( uid2 );
	assert( inref == "MPI" );
	interface_implementation = s->lookup_interface_implementation( uid2 );
	assert( interface_implementation == "OpenMPI" );

	uid2 = s->lookup_interfaceID( "MPI", "MPICH2" );

	assert( uid2 == uid );


	NodeID nid = s->register_nodeID( "testhost" );
	assert( s->lookup_nodeID( "testhost" ) == nid );
	assert( s->lookup_node_hostname( nid ) == "testhost" );

	DeviceID did = s->register_deviceID( nid, "/dev/ssd1" );
	assert( s->lookup_deviceID( nid, "/dev/ssd1" ) == did );
	assert( s->lookup_node_of_device( did ) == nid );
	assert( s->lookup_device_local_name( did ) == "/dev/ssd1" );


	FilesystemID fid = s->register_filesystemID( "pvfs2://10.0.0.1" );
	assert( fid == s->lookup_filesystemID( "pvfs2://10.0.0.1" ) );
	assert( s->lookup_filesystem_name( fid ) == "pvfs2://10.0.0.1" );


	UniqueComponentActivityID aid = s->register_activityID( uid, "open" );
	assert( aid == s->lookup_activityID( uid, "open" ) );
	try {
		s->lookup_activityID( uid, "open2" );
		assert( false );
	} catch( NotFoundError & e ) {

	}

	assert( s->lookup_activity_name( aid ) == "open" );
	assert( s->lookup_interface_of_activity( aid ) == uid );

	delete( s );
	delete( topology );

	cout << "OK" << endl;

	return 0;
}


