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

	//Test types
	TopologyType myType1, myType2;
	IGNORE_EXCEPTIONS( myType1 = topology->lookupTypeByName( "foo" ); );
	assert( !myType1 );
	IGNORE_EXCEPTIONS( myType1 = topology->lookupTypeById( 0 ); );
	assert( !myType1 );
	IGNORE_EXCEPTIONS( myType1 = topology->lookupTypeById( 1 ); );
	assert( !myType1 );

	myType1 = topology->registerType( "foo" );
	assert( myType1.name() == "foo" );
	assert( myType1.id() == 1 );

	IGNORE_EXCEPTIONS( myType2 = topology->lookupTypeById( 0 ); );
	assert( !myType2 );
	myType2 = topology->lookupTypeById( 1 );
	assert( myType2.name() == "foo" );
	myType2 = topology->lookupTypeByName( "foo" );
	assert( myType2.id() == 1 );

	//Test attributes
	TopologyAttribute myAttribute1, myAttribute2;
	IGNORE_EXCEPTIONS( myAttribute1 = topology->lookupAttributeByName( myType1, "foo" ); );
	assert( !myAttribute1 );
	IGNORE_EXCEPTIONS( myAttribute1 = topology->lookupAttributeById( 0 ); );
	assert( !myAttribute1 );
	IGNORE_EXCEPTIONS( myAttribute1 = topology->lookupAttributeById( 1 ); );
	assert( !myAttribute1 );

	myAttribute1 = topology->registerAttribute( myType1.id(), "foo", TopologyVariable::Type::FLOAT );
	assert( myAttribute1.name() == "foo" );
	assert( myAttribute1.id() == 1 );
	assert( myAttribute1.domainId() == myType1.id() );
	assert( myAttribute1.dataType() == TopologyVariable::Type::FLOAT );
	IGNORE_EXCEPTIONS( myAttribute2 = topology->registerAttribute( myType1.id(), "foo", TopologyVariable::Type::DOUBLE ); );
	assert( !myAttribute2 );

	IGNORE_EXCEPTIONS( myAttribute2 = topology->lookupAttributeById( 0 ); );
	assert( !myAttribute2 );
	myAttribute2 = topology->lookupAttributeById( 1 );
	assert( myAttribute2.name() == "foo" );
	assert( myAttribute2.domainId() == myType1.id() );
	assert( myAttribute2.dataType() == TopologyVariable::Type::FLOAT );
	myAttribute2 = topology->lookupAttributeByName( myType1.id(), "foo" );
	assert( myAttribute2.domainId() == myType1.id() );
	assert( myAttribute2.dataType() == TopologyVariable::Type::FLOAT );
	topology->registerAttribute( myType1.id(), "bar", TopologyVariable::Type::DOUBLE );

	delete topology;

	cerr << "OK" << endl;
	return 0;
}

