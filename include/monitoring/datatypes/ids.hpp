/** @file
 * This file contains structures and constructors for SIOX ID, e.g.
 * hardware, processes, components and attributes.
 * @authors Julian Kunkel
 */

#ifndef __SIOX_IDS_HPP
#define __SIOX_IDS_HPP

#include <core/datatypes/VariableDatatype.hpp>

#include <cstdint>
#include <vector>
#include <iostream>


using namespace std;

namespace monitoring{

/**
@page ids SIOX ID Overview
SIOX uses a number of different IDs, which are structured as follows:
@dot
digraph IDs
{
    // Settings
    //----------
    // Arrange from bottom to top
    rankdir="BT";
    // Defaults for nodes and edges
    node [shape=Mrecord];
    edge [style=solid];

    // The actual graph
    //------------------
    nid [label="NodeID\nuint32_t"]

    pid [label="{ProcessID|{<nid>nid\nNodeID|pid\nuint32_t|<time>time\nuint32_t}}", URL="\ref ProcessID"]
    nid:n -> pid:nid:s

    cid [label="{ComponentID|{<pid>pid\nProcessID|id\nuint16_t}}", URL="\ref ComponentID"]
    pid:n -> cid:pid:s

    aid [label="{ActivityID|{<cid>cid\nComponentID|id\nuint32_t}}", URL="\ref ActivityID"]
    cid:n -> aid:cid:s

    uiid [label="{UniqueInterfaceID|{interface\nuint16_t|implementation\nuint16_t}}", URL="\ref UniqueInterfaceID"]

    oaid [label="OntologyAttributeID\nuint32_t", URL="\ref OntologyAttributeID"]

    ucaid [label="UniqueComponentActivityID\nuint32_t", URL="\ref UniqueComponentActivityID"]

    assid [label="AssociatedID\nuint32_t", URL="\ref AssociateID"]

    rcid [label="{RemoteCallIentifier|{<nid>nid\nNodeID|<uiid>uiid\nUniqueInterfaceID|<assid>instance\nAssociatedID}}", URL="\ref RemoteCallIdentifier"]
    nid:n -> rcid:nid:s
    uiid:n -> rcid:uiid:s
    assid:n -> rcid:assid:s
}
@enddot
*/

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


/* Identifies a specific software interface, e.g. OpenMPI V3 
 * Globally unique => lookup in knowledge base is mandatory for each layer.
 * The config file for the layer may hold this additional information, so lookup comes for free.
 */
typedef uint32_t UniqueInterfaceID;

typedef VariableDatatype AttributeValue;

struct Attribute{
	OntologyAttributeID id;
	AttributeValue value;

    Attribute(){}
    Attribute(OntologyAttributeID id, const AttributeValue & value) : id(id), value(value){}
};

typedef uint64_t Timestamp;

typedef uint32_t ActivityError;


// The daemon fetches the NodeID from the knowledge base (or initiates creation if necessary)
// NodeID lookup_node_id(const char * hostname);
// See @TODO 

/* Software ID, identifying the application programm, may be a server as well */
struct ProcessID{
	NodeID nid;
	uint32_t pid;
	uint32_t time;

    inline bool operator==(ProcessID const & b) const{
        return memcmp(this, &b, sizeof(nid) + sizeof(pid) + sizeof(time));
    }   

    inline bool operator!=(ProcessID const & b) const{
        return ! (*this == b);
    }

    inline bool operator<(ProcessID const & b) const{
        return nid < b.nid || pid < b.pid || time < b.time;
    }
};


inline ostream& operator<<(ostream& os, const ProcessID & v){
    os << "(" << v.nid << "," << v.pid << "," << v.time << ")";
    return os;
}

// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
// RuntimeID create_runtime_id(NodeID 32 B,  getpid() 32B, gettimeofday(seconds) 32B );
// See @TODO




// The first 16 bit identify the interface, e.g. MPI2 or POSIX, the latter 16 the implementation version, e.g. MPICH2 vs. OpenMPI
// UniqueInterfaceID lookup_unique_interface_id(<InterfaceName>, <Version/implementation Name>);
// See @TODO


// What happens if one process uses one layer multiple times?
// Increase num...

/* Identifying a SIOX component */
struct ComponentID{
	ProcessID pid;
	//UniqueInterfaceID uiid;
	uint16_t id;

    inline bool operator==(ComponentID const & b) const{
        return memcmp(this, &b, sizeof(pid) + sizeof(id));
    }  

    inline bool operator!=(ComponentID const & b) const{
        return ! (*this == b);
    }

    inline bool operator<(ComponentID const & b) const{
        return id < b.id || pid < b.pid;
    }
};

inline ostream& operator<<(ostream& os, const ComponentID & v){
    os << "(" << v.pid << "," << v.id << ")";
    return os;
}

// TODO:
// ComponentID(siox_component){}
// ComponentID create_component_id(ProcessID 3*32 B, UIID + 16 bit);

struct RemoteCallIdentifier{
	// Several parameters assist matching of remote calls
	NodeID nid; // optional
	UniqueInterfaceID uuid; // optional
	AssociateID instance; // optional, remote call instance identifier

	RemoteCallIdentifier() {};
	RemoteCallIdentifier(NodeID n, UniqueInterfaceID u, AssociateID a) : nid(n), uuid(u), instance(a) {};
};

/* Identifying an activity */
struct ActivityID{
	ComponentID cid;
	uint32_t id;

   inline bool operator==(ActivityID const & b) const{
        return memcmp(this, &b, sizeof(cid) + sizeof(id));
    }   

    inline bool operator!=(ActivityID const & b) const{
        return ! (*this == b);
    }
};

inline ostream& operator<<(ostream& os, const ActivityID & v){
    os << "(" << v.cid << "," << v.id << ")";
    return os;
}


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

inline bool is_valid_id(ProcessID & id){
	return is_valid_id(id.nid);
}

inline bool is_valid_id(ComponentID & id){
	return is_valid_id(id.pid);
}

inline bool is_valid_id(ActivityID & id){
	return is_valid_id(id.cid);
}

}

#endif
