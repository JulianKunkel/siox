#ifndef SIOX_LL_INTERNAL_H_
#define SIOX_LL_INTERNAL_H_


#include <monitoring/datatypes/c-types.h>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>

#include <core/autoconfigurator/AutoConfigurator.hpp>


using namespace core;
using namespace monitoring;

// define all types for CPP
typedef AssociateID siox_associate;
typedef OntologyAttribute siox_attribute;
typedef UniqueComponentActivityID siox_component_activity;
typedef NodeID siox_node;
typedef UniqueInterfaceID siox_unique_interface;
typedef ComponentID siox_component;
typedef ActivityID siox_activity;
typedef RemoteCallID siox_remote_call;


extern "C"{
#include <monitoring/low-level-c/siox-ll.h>
}

/**
 * Implementation of the low-level API
 * The C-implementation serves as lightweight wrapper for C++ classes.
 */

struct process_info{
	NodeID nid;
	ProcessID pid;

	// Loaded ontology implementation
	monitoring::Ontology * ontology;
    // Loaded system information manager implementation
    monitoring::SystemInformationGlobalIDManager * system_information_manager;
    // Loaded association mapper implementation
    monitoring::AssociationMapper * association_mapper;

    // Contains all components
    core::ComponentRegistrar * registrar;

    // Loads all component modules
    core::AutoConfigurator * configurator;
};



// MZ: Obsolete?
// ProcessID forge_process_id(NodeID node, uint32_t pid, uint32_t time);

/*
 * Create a local ProcessID
 */
ProcessID create_process_id(NodeID node);


/*
 These functions are mainly used for testing purposes and since testing is based on C++ classes it is not useful to keep them.
 void siox_process_register(siox_nodeID * siox_nodeID, ProcessID * pid);
 void siox_process_finalize();
 */

#endif
