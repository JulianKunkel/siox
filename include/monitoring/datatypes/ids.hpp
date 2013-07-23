/**
 * This file contains structures and constructors for SIOX ID, e.g.
 * hardware, processes, components and attributes.
 * @Authors Julian Kunkel
 */

#ifndef __SIOX_IDS_HPP
#define __SIOX_IDS_HPP

#include "monitoring/datatypes/c-types.h"

#include <cstdint>
#include <vector>
#include <boost/variant.hpp>

using namespace std;

// Every hardware component which is addressable by the network is expected to have a unique (host)name.
// This name is translated to the NodeID.
// 
// In the following the approaches to create and/or lookup IDs is illustrated for each ID type.
// This includes necessary input data and a description who will create the ID and 
// where creation/lookup is implemented

/* Hardware ID, identifying the hardware component running on 
 * It is invalid if ID == 0
 * Type such as network card or storage node and model such as 10Gbit or vendor/ivybridge would identify the hardware
 * There are device ids 0x1014 for wireless and vendor ids such as 0x168C for Atheros already. We could use this coding or explicit description.
 */
typedef uint32_t NodeID;

typedef uint32_t DeviceID;

typedef uint32_t FilesystemID;

/* Identifies an attribute in the ontology */
typedef uint32_t OntologyAttributeID;

/* Idendifies a component specific activity in the ontology */
typedef uint32_t UniqueComponentActivityID;

/* The associate ID is valid only within a particular process */
typedef uint32_t AssociateID;

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, string, float, double> AttributeValue;

typedef struct {
	OntologyAttributeID id;
	AttributeValue value;
} Attribute;

/* Forward declaration */
struct ActivityID;

// The daemon fetches the NodeID from the knowledge base (or initiates creation if necessary)
// NodeID lookup_node_id(const char * hostname);
// See @TODO 

/* Software ID, identifying the application programm, may be a server as well */
typedef struct{
	NodeID nid;
	uint32_t pid;
	uint32_t time;
	vector<ActivityID *> activity_stack;
} ProcessID;
// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
// RuntimeID create_runtime_id(NodeID 32 B,  getpid() 32B, gettimeofday(seconds) 32B );
// See @TODO


/* Identifies a specific software interface, e.g. OpenMPI V3 
 * Globally unique => lookup in knowledge base is mandatory for each layer.
 * The config file for the layer may hold this additional information, so lookup comes for free.
 */
typedef struct{ 
	uint16_t interface; /*  It is invalid if interface == 0 */
	uint16_t implementation;
} UniqueInterfaceID;
// The first 16 bit identify the interface, e.g. MPI2 or POSIX, the latter 16 the implementation version, e.g. MPICH2 vs. OpenMPI
// UniqueInterfaceID lookup_unique_interface_id(<InterfaceName>, <Version/implementation Name>);
// See @TODO


// What happens if one process uses one layer multiple times?
// Increase num...

/* Identifying a SIOX component */
typedef struct{
	ProcessID pid;
	//UniqueInterfaceID uiid;
	uint16_t num;
	uint32_t next_activity_id;
} ComponentID;
// TODO:
// ComponentID(siox_component){}
// ComponentID create_component_id(ProcessID 3*32 B, UIID + 16 bit);
// The instance identifier such as "Port 4711" is relevant for matching of remote calls
// See @TODO

typedef struct {
	ActivityID *caller_aid;
	NodeID *target_node_id;
	UniqueInterfaceID *target_unique_interface_id;
	AssociateID *target_associate_id;
} RemoteCallID;

typedef struct {
	// Several parameters assist matching of remote calls
	NodeID hwid; // optional
	UniqueInterfaceID uuid; // optional
	AssociateID instance; // optional, remote call instance identifier
} RemoteCallIdentifier;

typedef struct {
	RemoteCallIdentifier target;
	vector<Attribute> attributeArray;
} RemoteCall;

/* Identifying an activity */
typedef struct ActivityID{
	uint32_t id;
	ComponentID *cid;
	UniqueComponentActivityID *caid;
	siox_timestamp t_start;
	siox_timestamp t_end;
	siox_activity_error error;
	RemoteCallIdentifier *remote_caller;
	vector<ActivityID> parent_activities;
	vector<Attribute> attributes;
	vector<RemoteCall> remote_calls;
} ActivityID;

// ActivityID create_activity_id(ComponentID 4*32 B, <Incrementing Counter>);
// See @TODO

// The identifier is fetched from the knowledge base
// OntologyAttributeID lookup_ontology_attribute(string uniqueOntologyIdentifier);
// OntologyAttributeID lookup_or_create_ontology_attribute(string uniqueOntologyIdentifier, string unit, enum STORAGE_TYPE)
// See @TODO

/////////////////////////////////////////////////////////////////////////////////////////
// Functions which check the validity of an optional ID

inline bool is_valid_id(uint32_t id){
	return id != 0;
}

inline bool is_valid_id(UniqueInterfaceID & id){
	return id.interface != 0;
}

inline bool is_valid_id(ProcessID & id){
	return is_valid_id(id.nid);
}

inline bool is_valid_id(ComponentID & id){
	return is_valid_id(id.pid);
}

inline bool is_valid_id(ActivityID & id){
	return is_valid_id(*(id.cid));
}

#endif
