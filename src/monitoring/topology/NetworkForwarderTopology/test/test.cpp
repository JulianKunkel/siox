/**
 * @file test.cpp
 *
 * @author Nathanael Hübbe
 * @author Roman Michel
 * @author Julian Kunkel
 * @date   2014
 */

// Define WRITE_ONLY here to skip reading ops for benchmark reasons
// Doing so will make the test itself mostly useless
// #define WRITE_ONLY

// threadcount defines how many times the test will run consecutively
const int threadcount = 1;

#include <iostream>

#include <util/ExceptionHandling.hpp>
#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>
#include <boost/thread.hpp>
#include <monitoring/topology/Topology.hpp>
#include <core/comm/NetworkService.hpp>

#include "../NetworkForwarderTopologyOptions.hpp"
#include "../NetworkForwarderTopologyServerOptions.hpp"
#include "../NetworkForwarderCommunication.hpp"

using namespace core;
using namespace monitoring;

void workerFunc(int i) {

	// start one communication module
	CommunicationModule * commModule = module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(commModule != nullptr);
	commModule->init();
	commModule->start();

	// Create a backend topology for the server
	Topology * backend = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	backend->init();
	backend->start();

	// Start the server first
	Component * server = core::module_create_instance<Component>( "", "siox-monitoring-NetworkForwarderTopologyServer", NETWORK_SERVICE_INTERFACE );

	NetworkForwarderTopologyServerOptions & o = server->getOptions<NetworkForwarderTopologyServerOptions>();

	o.comm = commModule;
    o.commAddress = "127.0.0.1:8080";
    o.topologyBackend = backend;

    // TODO: start und stop implementieren
    server->init();
    server->start();

	// Start the client
	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-NetworkForwarderTopology", MONITORING_TOPOLOGY_INTERFACE );

    NetworkForwarderTopologyOptions & o2 = topology->getOptions<NetworkForwarderTopologyOptions>();
    o2.comm = commModule;
    o2.targetAddress = "127.0.0.1:8080";

    topology->init();
    topology->start();

	//Test types
	TopologyType type1, type2;
	#ifndef WRITE_ONLY
	type1 = topology->lookupTypeByName( "type1234"+to_string(i) );
	assert( !type1 );

	type1 = topology->lookupTypeById( 0 );
	assert( !type1 );

	type1 = topology->lookupTypeById( 4711 );
	assert( !type1 );
	#endif

	type1 = topology->registerType( "type1"+to_string(i) );
	assert( type1.name() == "type1"+to_string(i) );

	#ifndef WRITE_ONLY
	type2 = topology->lookupTypeByName( "type1"+to_string(i) );

	assert(type2);
	assert( type2.name() == "type1"+to_string(i) );

	type2 = topology->lookupTypeById( type2.id() );
	assert( type2.name() == "type1"+to_string(i) );
	#endif

	type2 = topology->registerType( "type2"+to_string(i) );
	assert( type2.name() == "type2"+to_string(i) );

	//Test objects
	TopologyObject object1, object2;
	#ifndef WRITE_ONLY
	object1 = topology->lookupObjectById( 0 );
	assert( !object1 );
	object1 = topology->lookupObjectById( 4711 );
	assert( !object1 );
	#endif

	object1 = topology->registerObject( 0, type2.id(), "object1"+to_string(i), type1.id() );
	assert( object1.type() == type1.id() );

	#ifndef WRITE_ONLY
	object2 = topology->lookupObjectById( object1.id() );
	assert( object2.type() == object1.type() );
	#endif

	object2 = topology->registerObject( object1.id(), type1.id(), "object2"+to_string(i), type2.id() );
	assert( object2.type() == type2.id() );

	//Test relations
	TopologyRelation relation1, relation2, relation3, relation4, relation5;

	#ifndef WRITE_ONLY
	relation1 = topology->lookupRelation( 0, type1.id(), "foo"+to_string(i) );
	assert( !relation1 );

	relation1 = topology->lookupRelation( object2.id(), type2.id(), "object123"+to_string(i) );
	assert( !relation1 );
	relation1 = topology->lookupRelation( object1.id(), type2.id(), "object123"+to_string(i) );
	assert( !relation1 );
	relation1 = topology->lookupRelation( 0, type1.id(), "object232"+to_string(i) );
	assert( !relation1 );

	relation1 = topology->lookupRelation( 0, type2.id(), "object1"+to_string(i) );
	assert( relation1.childName() == "object1"+to_string(i) );
	assert( relation1.parent() == 0 );
	assert( relation1.child() == object1.id() );
	assert( relation1.type() == type2.id() );

	relation2 = topology->lookupRelation( object1.id(), type1.id(), "object2"+to_string(i) );
	assert( relation2.childName() == "object2"+to_string(i) );
	assert( relation2.parent() == object1.id() );
	assert( relation2.child() == object2.id() );
	assert( relation2.type() == type1.id() );
	#endif

	relation3 = topology->registerRelation( 0, type1.id(), "object2"+to_string(i), object2.id() );
	assert( relation3.child() == object2.id() );
	assert( relation3.type() == type1.id() );

	relation4 = topology->registerRelation( object2.id(), type2.id(), "object1"+to_string(i), object1.id() );
	assert( relation4.child() == object1.id() );
	assert( relation4.type() == type2.id() );

	relation5 = topology->registerRelation( object2.id(), type2.id(), "@object1/test2:schuh"+to_string(i), object1.id() );
	assert( relation5.child() == object1.id() );
	assert( relation5.type() == type2.id() );

	#ifndef WRITE_ONLY
	relation5 = topology->lookupRelation( object2.id(), type2.id(), "@object1/test2:schuh"+to_string(i));
	assert( relation5.child() == object1.id() );
	assert( relation5.type() == type2.id() );
	#endif

	//Test attributes
	TopologyAttribute attribute1, attribute2, attribute3;

	#ifndef WRITE_ONLY
	attribute1 = topology->lookupAttributeByName( type1, "attribute123"+to_string(i) );
	assert( !attribute1 );
	attribute1 = topology->lookupAttributeById( 0 );
	assert( !attribute1 );
	attribute1 = topology->lookupAttributeById( 4711 );
	assert( !attribute1 );
	#endif

	attribute1 = topology->registerAttribute( type1.id(), "attribute1"+to_string(i), TopologyVariable::Type::FLOAT );
	assert( attribute1.name() == "attribute1"+to_string(i) );
	assert( attribute1.domainId() == type1.id() );
	assert( attribute1.dataType() == TopologyVariable::Type::FLOAT );

	#ifndef WRITE_ONLY
	attribute2 = topology->lookupAttributeById( attribute1.id() );
	assert( attribute2.name() == "attribute1"+to_string(i) );
	assert( attribute2.domainId() == type1.id() );
	assert( attribute2.dataType() == TopologyVariable::Type::FLOAT );

	attribute2 = topology->lookupAttributeByName( type1.id(), "attribute1"+to_string(i) );
	assert( attribute2.domainId() == type1.id() );
	assert( attribute2.dataType() == TopologyVariable::Type::FLOAT );
	#endif

	attribute2 = topology->registerAttribute( type1.id(), "attribute2"+to_string(i), TopologyVariable::Type::DOUBLE );

	attribute3 = topology->registerAttribute( type1.id(), "attribute3"+to_string(i), TopologyVariable::Type::STRING );

	TopologyValue value1, value2;




	assert(topology->setAttribute( object1.id(), attribute2.id(), TopologyVariable( 3.0 ) ));

	#ifndef WRITE_ONLY
	value2 = topology->getAttribute( object1.id(), attribute2.id() );
	assert( value2.object() == object1.id() );
	assert( value2.attribute() == attribute2.id() );
	assert( value2.value() == 3.0 );
	#endif

	assert ( topology->setAttribute( object1.id(), attribute1.id(), TopologyVariable( 1.1f ) ));
	#ifndef WRITE_ONLY
	value2 = topology->getAttribute( object1.id(), attribute1.id() );
	assert( value2.value() == 1.1f );
	#endif

	assert(topology->setAttribute( object1.id(), attribute2.id(), TopologyVariable( 2.0 ) ));
	#ifndef WRITE_ONLY
	value2 = topology->getAttribute( object1.id(), attribute2.id() );
	assert( value2.object() == object1.id() );
	assert( value2.attribute() == attribute2.id() );
	assert( value2.value() == 2.0 );
	#endif


	assert(topology->setAttribute( object1.id(), attribute3.id(), "/home/test/fritz@schuh:/"+to_string(i) ));
	#ifndef WRITE_ONLY
	value2 = topology->getAttribute( object1.id(), attribute3.id() );
	assert( value2.object() == object1.id() );
	assert( value2.attribute() == attribute3.id() );
	assert( value2.value() == "/home/test/fritz@schuh:/"+to_string(i) );
	#endif

	topology->stop();

	delete topology;
}

int main( int argc, char const * argv[] ) throw() {
	uint ops = 0;
   struct timeval start, end;

   long mtime, seconds, useconds;

	boost::thread* threads[threadcount];

	gettimeofday(&start, NULL);

	if (threadcount > 1) {

		for (int i = 0; i<threadcount; i++) {
			threads[i] = new boost::thread(workerFunc, i);
		}

		for (int i = 0; i<threadcount; i++) {
			threads[i]->join();
		}
	}else{
		workerFunc(1);
	}

	gettimeofday(&end, NULL);

	// These values need to be adjusted when the test above changes
	#ifdef WRITE_ONLY
	ops = 14*threadcount;
	#else
	ops = 40*threadcount;
	#endif

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	printf("%i Ops in %ld milliseconds\n", ops, mtime);

	cerr << "OK" << endl;
	return 0;
}

