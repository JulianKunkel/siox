/**
 * This file contains structures and constructors for SIOX ID, e.g.
 * hardware, processes, components and attributes.
 * @Authors Julian Kunkel Marc Wiedemann
 */

#ifndef __SIOX_IDS_HPP
#define __SIOX_IDS_HPP

#include <stdint.h>

// In the following the approaches to create and/or lookup IDs is illustrated for each ID type.
// This includes necessary input data and a description who will create the ID and 
// where creation/lookup is implemented

/* Hardware ID, identifying the hardware component running on 
 * It is invalid if ID == 0
 * Type such as network card or storage node and model such as 10Gbit or vendor/ivybridge would identify the hardware
 * There are device ids 0x1014 for wireless and vendor ids such as 0x168C for Atheros already. We could use this coding or explicit description.
 */
typedef  struct{
	uint32_t numerical identifier;
	string type;
	model description;
} HwID;
// The daemon fetches the HwID from the knowledge base (or initiates creation if necessary)
// HwID lookup_hardware_id(const char * hostname);
// See @TODO

/* Software ID, identifying the application programm, may be a server as well */
typedef struct{
//	HwID hw; // necessary to connect here?
	uint32_t pid;
	uint32_t time;
} ProcessID;
// Each process can create a runtime ID locally KNOWING the HwID from the daemon
// RuntimeID create_runtime_id(HwID 32 B,  getpid() 32B, gettimeofday(seconds) 32B );
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


/* Identifying a SIOX component */
typedef struct{  
	ProcessID pid;
	UniqueInterfaceID uiid;
} ComponentID;
// ComponentID create_component_id(ProcessID 3*32 B, UIID);
// The instance identifier such as "Port 4711" is relevant for matching of remote calls
// See @TODO

/* Identifying an activity */
typedef struct{
	ComponentID cid;
	uint32_t num;
} ActivityID;
// ActivityID create_activity_id(ComponentID 4*32 B, <Incrementing Counter>);
// See @TODO

/* Identifies an attribute in the ontology */
typedef uint32_t OntologyAttributeID;
// The identifier is fetched from the knowledge base
// OntologyAttributeID lookup_ontology_attribute(string uniqueOntologyIdentifier);
// OntologyAttributeID lookup_or_create_ontology_attribute(string uniqueOntologyIdentifier, string unit, enum STORAGE_TYPE)
// See @TODO


/////////////////////////////////////////////////////////////////////////////////////////
// Functions which check the validity of an optional ID

#ifdef __cplusplus

inline bool is_valid_id(uint32_t id){
	return id != 0;
}

inline bool is_valid_id(UniqueInterfaceID & id){
	return id.interface != 0;
}

inline bool is_valid_id(ProcessID & id){
	return is_valid_id(id.hw);
}

inline bool is_valid_id(ComponentID & id){
	return is_valid_id(id.pid);
}

inline bool is_valid_id(ActivityID & id){
	return is_valid_id(id.cid);
}

#endif

#endif
