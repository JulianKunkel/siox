#ifndef SIOX_LL_INTERNAL_H_
#define SIOX_LL_INTERNAL_H_


#include <monitoring/low-level-c/siox-ll.h>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>

#include <core/autoconfigurator/AutoConfigurator.hpp>

/**
 * Implementation of the low-level API
 * The C-implementation serves as lightweight wrapper for C++ classes.
 */

struct process_info{
	HwID hwid;
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



ProcessID build_process_id(HwID hw, uint32_t pid, 	uint32_t time);


#endif