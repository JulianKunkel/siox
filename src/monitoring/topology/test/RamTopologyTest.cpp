/**
 * @file RamTopologyTest.cpp
 *
 * @author Nathanael Hübbe
 * @date   2013
 */

#include <iostream>

#include <util/ExceptionHandling.hpp>
#include <core/module/ModuleLoader.hpp>
#include <monitoring/topology/Topology.hpp>

#include "../RamTopologyOptions.hpp"

using namespace core;
using namespace monitoring;

int main( int argc, char const * argv[] ) throw() {
	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	topology->getOptions<RamTopologyOptions>();
	topology->init();

	const Topology::Type* myType = NULL, *myType2 = NULL;
	IGNORE_EXCEPTIONS( myType = &topology->lookupTypeByName( "foo" ); );
	assert( !myType );
	IGNORE_EXCEPTIONS( myType = &topology->lookupTypeById( 0 ); );
	assert( !myType );
	IGNORE_EXCEPTIONS( myType = &topology->lookupTypeById( 1 ); );
	assert( !myType );

	myType = &topology->registerType( "foo" );
	assert( myType->name == "foo" );
	assert( myType->id == 1 );

	IGNORE_EXCEPTIONS( myType2 = &topology->lookupTypeById( 0 ); );
	assert( !myType2 );
	myType2 = &topology->lookupTypeById( 1 );
	assert( myType2 == myType );
	myType2 = &topology->lookupTypeByName( "foo" );
	assert( myType2 == myType );

	delete topology;

	cerr << "OK" << endl;
	return 0;
}

