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
	TopologyType type1, type2;
	type1 = topology->lookupTypeByName( "type1" );
	assert( !type1 );
	type1 = topology->lookupTypeById( 0 );
	assert( !type1 );
	type1 = topology->lookupTypeById( 1 );
	assert( !type1 );

	type1 = topology->registerType( "type1" );
	assert( type1.name() == "type1" );
	assert( type1.id() == 1 );

	type2 = topology->lookupTypeById( 0 );
	assert( !type2 );
	type2 = topology->lookupTypeById( 1 );
	assert( type2.name() == "type1" );
	type2 = topology->lookupTypeByName( "type1" );
	assert( type2.id() == 1 );

	type2 = topology->registerType( "type2" );
	assert( type2.name() == "type2" );
	assert( type2.id() == 2 );

	//Test objects
	TopologyObject object1, object2;
	object1 = topology->lookupObjectById( 0 );
	assert( !object1 );
	object1 = topology->lookupObjectById( 1 );
	assert( !object1 );

	object1 = topology->registerObject( 0, type2.id(), "object1", type1.id() );
	assert( object1.id() == 1 );
	assert( object1.type() == type1.id() );
	object2 = topology->lookupObjectById( 0 );
	assert( !object2 );
	object2 = topology->lookupObjectById( 1 );
	assert( object2.type() == type1.id() );
	object2 = topology->registerObject( object1.id(), type1.id(), "object2", type2.id() );
	assert( object2.id() == 2 );
	assert( object2.type() == type2.id() );

	//Test relations
	TopologyRelation relation1, relation2, relation3, relation4;
	relation1 = topology->lookupRelation( 0, type1.id(), "foo" );
	assert( !relation1 );
	relation1 = topology->lookupRelation( object2.id(), type2.id(), "object1" );
	assert( !relation1 );
	relation1 = topology->lookupRelation( object1.id(), type2.id(), "object1" );
	assert( !relation1 );
	relation1 = topology->lookupRelation( 0, type1.id(), "object2" );
	assert( !relation1 );
	relation1 = topology->lookupRelation( 0, type1.id(), "object1" );
	assert( !relation1 );
	relation1 = topology->lookupRelation( 0, type2.id(), "object1" );
	assert( relation1.childName() == "object1" );
	assert( relation1.parent() == 0 );
	assert( relation1.child() == object1.id() );
	assert( relation1.type() == type2.id() );
	relation2 = topology->lookupRelation( object1.id(), type1.id(), "object2" );
	assert( relation2.childName() == "object2" );
	assert( relation2.parent() == object1.id() );
	assert( relation2.child() == object2.id() );
	assert( relation2.type() == type1.id() );

	relation3 = topology->registerRelation( 0, type2.id(), "object1", object2.id() );
	assert( !relation3 );
	relation3 = topology->registerRelation( 0, type2.id(), "object1", object1.id() );
	assert( &*relation3 == &*relation1 );
	relation3 = topology->registerRelation( 0, type1.id(), "object2", object2.id() );
	assert( relation3.child() == object2.id() );
	assert( relation3.type() == type1.id() );
	relation4 = topology->registerRelation( object2.id(), type2.id(), "object1", 0 );
	assert( !relation4 );
	relation4 = topology->registerRelation( object2.id(), type2.id(), "object1", object1.id() );
	assert( relation4.child() == object1.id() );
	assert( relation4.type() == type2.id() );

	{
		TopologyObject temp, object3;
		temp = topology->lookupObjectByPath( "" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "/object1" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "object1/" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "object1//object2" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "type1:object1" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "type2:object1" );
		assert( &*temp == &*object1 );
		temp = topology->lookupObjectByPath( "type1:object2/type2:object1" );
		assert( &*temp == &*object1 );
		temp = topology->lookupObjectByPath( "type1:object2" );
		assert( &*temp == &*object2 );
		temp = topology->lookupObjectByPath( "type2:object1/type1:object2" );
		assert( &*temp == &*object2 );
		object3 = topology->registerObjectByPath( "type1:object2:type2/tic:tac/toe:tric:trac" );
		temp = topology->lookupObjectByPath( "type1:object2/tic:tac/toe:tric" );
		assert( &*temp == &*object3 );
		temp = topology->registerObjectByPath( "type1:object2:type2/tic:tac/toe:tric:trac" );
		assert( &*temp == &*object3 );
		temp = topology->registerObjectByPath( "type1:object2/tic:tac/toe:tric:trac" );
		assert( !temp );
		temp = topology->registerObjectByPath( "type1:object2:type2/tic:tac/toe:tric" );
		assert( !temp );
		temp = topology->lookupObjectByPath( "type1:object2:type2/tic:tac/toe:tric:trac" );
		assert( &*temp == &*object3 );
		assert( topology->setAlias( "@foo", "type1:object2" ) );
		assert( topology->setAlias( "@foo", "type1:object2" ) );
		assert( !topology->setAlias( "@foo", "foo:bar" ) );
		assert( !topology->setAlias( "@bar", "foo" ) );
		assert( !topology->setAlias( "@bar", "foo:bar/tic:tac" ) );
		assert( !topology->setAlias( "foo", "type1:object2" ) );
		assert( !topology->setAlias( "@foo/bar", "type1:object2" ) );
		assert( topology->setAlias( "@bar", "@foo" ) );
		temp = topology->lookupObjectByPath( "@bar/tic:tac/toe:tric" );
		assert( &*temp == &*object3 );
	}

	{
		TopologyRelationList list;
		list = topology->enumerateChildren( 0, 0 );
		assert( list.size() == 2 );
		assert( &*list[0] == &*relation1 || &*list[0] == &*relation3 );
		assert( &*list[1] == &*relation1 || &*list[1] == &*relation3 );
		list = topology->enumerateChildren( 0, type1.id() );
		assert( list.size() == 1 );
		assert( &*list[0] == &*relation3 );
		list = topology->enumerateChildren( object1.id(), 0 );
		assert( list.size() == 1 );
		assert( &*list[0] == &*relation2 );
		list = topology->enumerateChildren( object1.id(), type2.id() );
		assert( !list.size() );

		list = topology->enumerateParents( 0, 0 );
		assert( !list.size() );
		list = topology->enumerateParents( 0, type1.id() );
		assert( !list.size() );
		list = topology->enumerateParents( object1.id(), 0 );
		assert( list.size() == 2 );
		assert( &*list[0] == &*relation1 || &*list[0] == &*relation4 );
		assert( &*list[1] == &*relation1 || &*list[1] == &*relation4 );
		list = topology->enumerateParents( object1.id(), type1.id() );
		assert( !list.size() );
	}

	//Test attributes
	TopologyAttribute attribute1, attribute2;
	attribute1 = topology->lookupAttributeByName( type1, "attribute1" );
	assert( !attribute1 );
	attribute1 = topology->lookupAttributeById( 0 );
	assert( !attribute1 );
	attribute1 = topology->lookupAttributeById( 1 );
	assert( !attribute1 );

	attribute1 = topology->registerAttribute( type1.id(), "attribute1", TopologyVariable::Type::FLOAT );
	assert( attribute1.name() == "attribute1" );
	assert( attribute1.id() == 1 );
	assert( attribute1.domainId() == type1.id() );
	assert( attribute1.dataType() == TopologyVariable::Type::FLOAT );
	attribute2 = topology->registerAttribute( type1.id(), "attribute1", TopologyVariable::Type::DOUBLE );
	assert( !attribute2 );

	attribute2 = topology->lookupAttributeById( 0 );
	assert( !attribute2 );
	attribute2 = topology->lookupAttributeById( 1 );
	assert( attribute2.name() == "attribute1" );
	assert( attribute2.domainId() == type1.id() );
	assert( attribute2.dataType() == TopologyVariable::Type::FLOAT );
	attribute2 = topology->lookupAttributeByName( type1.id(), "attribute1" );
	assert( attribute2.domainId() == type1.id() );
	assert( attribute2.dataType() == TopologyVariable::Type::FLOAT );
	attribute2 = topology->registerAttribute( type1.id(), "attribute2", TopologyVariable::Type::DOUBLE );

	TopologyVariable value1(1.0f);
	TopologyValue value2;
	// assert( ! topology->setAttribute( object1.id(), attribute1.id(), TopologyVariable(1) ) );
	assert( topology->setAttribute( object1.id(), attribute1.id(), value1) );
	value2 = topology->getAttribute( object2.id(), attribute1.id() );
	assert( !value2 );
	value2 = topology->getAttribute( object1.id(), attribute2.id() );
	assert( !value2 );
	value2 = topology->getAttribute( object1.id(), attribute1.id() );
	assert( value1 == value2.value() );

	topology->setAttribute( object1.id(), attribute2.id(), TopologyVariable( 1.0 ) );
	value2 = topology->getAttribute( object1.id(), attribute2.id() );
	assert( value2.object() == object1.id() );
	assert( value2.attribute() == attribute2.id() );
	assert( value2.value() == 1.0 );

	TopologyValueList valueList;
	valueList = topology->enumerateAttributes( object2.id() );
	assert( !valueList.size() );
	valueList = topology->enumerateAttributes( object1.id() );
	assert( valueList.size() == 2 );
	assert( valueList[0].value() == value1 || &*valueList[0] == &*value2 );
	assert( valueList[1].value() == value1 || &*valueList[1] == &*value2 );

	// Test object with full string paths
	{
	TopologyObject obj1 = topology->registerObjectByPath( {{"ComponentID", "test", "Component"}, {"AttributeID", 4711, "Attribute"}} );
	assert(obj1);
	TopologyObject obj2 = topology->lookupObjectByPath( {{"ComponentID", "test"}, {"AttributeID", 4711}} );
	assert( &*obj1 == &*obj2 );
	}
	{
	TopologyObject obj1 = topology->registerObjectByPath( {{"ComponentID", "@proc/schuh:shoe", "Component"}, {"AttributeID", "@slang:///FALSE\1\2\3TRUE", "Attribute"}} );
	assert(obj1);
	TopologyObject obj2 = topology->lookupObjectByPath(  {{"ComponentID", "@proc/schuh:shoe"}, {"AttributeID", "@slang:///FALSE\1\2\3TRUE"}} );
	assert( &*obj1 == &*obj2 );
	}


	delete topology;

	cerr << "OK" << endl;
	return 0;
}

