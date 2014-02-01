/**
 * @file siox-ll-internal.hpp
 *
 * Additional header file for the siox low-level API.
 * For internal use only.
 * @date 2013-08-06
 */

#ifndef SIOX_LL_INTERNAL_H_
#define SIOX_LL_INTERNAL_H_

#include <boost/thread/shared_mutex.hpp>

#include <C/siox-types.h>

#include <core/autoconfigurator/AutoConfigurator.hpp>
#include <util/OverheadStatistics.hpp>

#include <monitoring/datatypes/ids.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_builder/ActivityBuilder.hpp>

#include <knowledge/optimizer/Optimizer.hpp>


using namespace core;
using namespace monitoring;
using namespace util;

typedef RemoteCall siox_remote_call;


struct siox_component {
	ComponentID cid;
	UniqueInterfaceID uid;
	AssociateID instance_associate;

	ActivityMultiplexer * amux;
	// We need one ActivityBuilder per thread, independent of component
};


struct siox_activity {
	Activity * activity;
	siox_component * component;

	siox_activity( Activity * a, siox_component * c ) : activity( a ), component( c ) {}
};

struct process_info {
	NodeID nid;
	ProcessID pid;

	/// Loaded ontology implementation
	monitoring::Ontology * ontology;
	/// Loaded system information manager implementation
	monitoring::SystemInformationGlobalIDManager * system_information_manager;
	/// Loaded association mapper implementation
	monitoring::AssociationMapper * association_mapper;

	/// Optional: the optimizer.
	knowledge::Optimizer * optimizer;

	/// Loaded activity multiplexer implementation
	ActivityMultiplexer * amux;

	/// Contains all components
	core::ComponentRegistrar * registrar;

	/// Loads all component modules
	core::AutoConfigurator * configurator;

	/// Overhead statistics
	util::OverheadStatistics * overhead;

	/// Protect critical datastructures
	boost::shared_mutex  critical_mutex;

	uint16_t last_componentID;
};


/*
 * Create a local ProcessID
 */
ProcessID create_process_id( NodeID node );


/*
 These functions are mainly used for testing purposes and since testing is based on C++ classes it is not useful to keep them.
 void siox_process_register(siox_nodeID * siox_nodeID, ProcessID * pid);
 void siox_process_finalize();
 */

#endif
