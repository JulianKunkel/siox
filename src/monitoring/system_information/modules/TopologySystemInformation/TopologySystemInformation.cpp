#include <string>
#include <sstream>

#include <monitoring/system_information/SystemInformationGlobalIDManagerImplementation.hpp>
#include <monitoring/system_information/modules/TopologySystemInformation/TopologySystemInformationOptions.hpp>
#include <monitoring/topology/Topology.hpp>


using namespace std;

using namespace monitoring;
using namespace core;

namespace monitoring {
class TopologySystemInformation: public SystemInformationGlobalIDManager {
private:
	Topology * topology = nullptr;
	TopologyObjectId pluginTopoObjectID;
	
	TopologyTypeId hostID;
	TopologyTypeId deviceID;
	TopologyTypeId fsID;
	TopologyTypeId interfaceID;
	TopologyTypeId activityID;

	TopologyAttributeId attrNameID;

	TopologyAttributeId attrInterfaceNameID;
	TopologyAttributeId attrUNIDID;
	TopologyAttributeId attrImplementationNameID;
	TopologyAttributeId attrNodeID;
	TopologyAttributeId attrDeviceNameID;
	TopologyAttributeId attrFSNameID;

	void init() {
		TopologySystemInformationOptions & o = getOptions<TopologySystemInformationOptions>();
		topology = GET_INSTANCE(Topology, o.topology);

		assert(topology);

		TopologyObject ontologyTopologyObject = topology->registerObjectByPath( "Module:SystemInformationIDManager" );
		pluginTopoObjectID = ontologyTopologyObject.id();		

		TopologyTypeId systemInfoID = topology->registerType("SystemInformation").id();

		hostID = topology->registerType("SystemInformationHost").id();
		deviceID = topology->registerType("SystemInformationDevice").id();
		fsID = topology->registerType("SystemInformationFilesystem").id();
		interfaceID = topology->registerType("SystemInformationInterface").id();
		activityID = topology->registerType("SystemInformationActivity").id();

		attrNameID = topology->registerAttribute( systemInfoID, "Name", VariableDatatype::Type::STRING ).id();

		attrInterfaceNameID = topology->registerAttribute( systemInfoID, "Interface", VariableDatatype::Type::STRING ).id();
		attrImplementationNameID = topology->registerAttribute( systemInfoID, "Implementation", VariableDatatype::Type::STRING ).id();
	 	attrNodeID = topology->registerAttribute( systemInfoID, "Node", VariableDatatype::Type::UINT32 ).id();
	 	attrUNIDID = topology->registerAttribute( systemInfoID, "UNID", VariableDatatype::Type::UINT32 ).id();
	 	attrDeviceNameID = topology->registerAttribute( systemInfoID, "LocalName", VariableDatatype::Type::STRING ).id();
	 	attrFSNameID = topology->registerAttribute( systemInfoID, "GlobalName", VariableDatatype::Type::STRING ).id();
	}

	ComponentOptions * AvailableOptions(){
		return new TopologySystemInformationOptions();
	}

	NodeID register_nodeID( const string & hostname ) {
		TopologyObject obj = topology->registerObject( pluginTopoObjectID, hostID, hostname, hostID);
		if ( ! obj ){
			throw IllegalStateError("Could not register node: " + hostname);
		}

		topology->setAttribute( obj.id(), attrNameID, hostname );

		return obj.id();
	}

	NodeID lookup_nodeID( const string & hostname ) const throw( NotFoundError ) {
		TopologyRelation tr = topology->lookupRelation( pluginTopoObjectID, hostID, hostname );
		if (! tr){
			throw NotFoundError();
		}
		return tr.child();
	}

	const string lookup_node_hostname( NodeID id ) const throw( NotFoundError ) {
		TopologyObject obj = topology->lookupObjectById( id );
		if (! obj){
			throw NotFoundError();
		}

		TopologyValue tv = topology->getAttribute( obj.id(), attrNameID );
		if (! tv){
			throw NotFoundError();
		}

		return tv.value().str();

	}


	DeviceID register_deviceID( NodeID id, const string & local_unique_identifier ) {
		TopologyObject d = topology->registerObject( id, deviceID, local_unique_identifier, deviceID);
		if ( ! d ){
			throw IllegalStateError("Could not register device: " + local_unique_identifier);
		}

		topology->setAttribute( d.id(), attrNodeID, id );
		topology->setAttribute( d.id(), attrDeviceNameID, local_unique_identifier );

		return d.id();
	}

	DeviceID lookup_deviceID( NodeID id, const string & local_unique_identifier ) const throw( NotFoundError ) {
		TopologyRelation tr = topology->lookupRelation(id, deviceID, local_unique_identifier );
		if (! tr){
			throw NotFoundError();
		}
		return tr.child();
	}


	FilesystemID register_filesystemID( const string & global_unique_identifier ) {
		TopologyObject d = topology->registerObject( pluginTopoObjectID, fsID, global_unique_identifier, fsID);
		if ( ! d ){
			throw IllegalStateError("Could not register filesystem: " + global_unique_identifier);
		}
		topology->setAttribute(d.id(), attrFSNameID, global_unique_identifier);		

		return d.id();
	}

	FilesystemID lookup_filesystemID( const string & global_unique_identifier ) const throw( NotFoundError ) {
		TopologyRelation tr = topology->lookupRelation(pluginTopoObjectID, fsID, global_unique_identifier );
		if (! tr){
			throw NotFoundError();
		}
		return tr.child();		
	}


	UniqueComponentActivityID register_activityID( UniqueInterfaceID id, const string & name ) {
		TopologyObject d = topology->registerObject( id, activityID, name, activityID);
		if ( ! d ){
			throw IllegalStateError("Could not register name: " + name);
		}

		topology->setAttribute(d.id(), attrNameID, name);
		topology->setAttribute(d.id(), attrUNIDID, id);

		return d.id();
	}

	UniqueComponentActivityID lookup_activityID( UniqueInterfaceID id, const string & name ) const throw( NotFoundError ) {
		TopologyRelation tr = topology->lookupRelation(id, activityID, name );
		if (! tr){
			throw NotFoundError();
		}
		return tr.child();		
	}

	UniqueInterfaceID register_interfaceID( const string & interface, const string & implementation ) {
		stringstream s;
		s << "SystemInformationInterface:" << interface << "/SystemInformationInterfaceImplementation:" << implementation;

		TopologyObject d = topology->registerObjectByPath( s.str(), pluginTopoObjectID );
		if ( ! d ){
			throw IllegalStateError("Could not register interface: " + s.str());
		}
		const TopologyObjectId objID = d.id();

		topology->setAttribute(objID, attrImplementationNameID, implementation);
		topology->setAttribute(objID, attrInterfaceNameID, interface);

		return d.id();
	}

	UniqueInterfaceID lookup_interface_of_activity( UniqueComponentActivityID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrUNIDID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().uint32();
	}

	UniqueInterfaceID lookup_interfaceID( const string & interface, const string & implementation ) const throw( NotFoundError ) {
		stringstream s;
		s << "SystemInformationInterface:" << interface << "/SystemInformationInterfaceImplementation:" << implementation;
		TopologyObject obj = topology->lookupObjectByPath( s.str(), pluginTopoObjectID);
		if (! obj){
			throw NotFoundError();
		}
		return obj.id();
	}

	NodeID lookup_node_of_device( DeviceID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrNodeID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().uint32();
	}


	const string lookup_device_local_name( DeviceID id ) const throw( NotFoundError ) {
				TopologyValue tv = topology->getAttribute(id, attrDeviceNameID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().str();
	}

	const string lookup_filesystem_name( FilesystemID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrFSNameID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().str();		
	}

	const string lookup_activity_name( UniqueComponentActivityID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrNameID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().str();		
	}


	const string lookup_interface_name( UniqueInterfaceID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrInterfaceNameID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().str();		
	}

	const string lookup_interface_implementation( UniqueInterfaceID id ) const throw( NotFoundError ) {
		TopologyValue tv = topology->getAttribute(id, attrImplementationNameID);
		if ( ! tv ){
			throw NotFoundError();
		}
		return tv.value().str();		
	}
};

}



extern "C" {
	void * MONITORING_SYSTEMINFORMATION_GLOBAL_ID_INSTANCIATOR_NAME()
	{
		return new TopologySystemInformation();
	}
}
