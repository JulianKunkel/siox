#include <assert.h>
#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/association_mapper/modules/TopologyAssociationMapper/TopologyAssociationMapperOptions.hpp>

using namespace std;

using namespace monitoring;
using namespace core;

#define myAssert(x) assert(x && "line: " && __LINE__ )

int main( int argc, char const * argv[] )
{
	AssociationMapper * o = core::module_create_instance<AssociationMapper>( "", "siox-monitoring-TopologyAssociationMapper", MONITORING_ASSOCIATION_MAPPER_INTERFACE );
	myAssert( o != nullptr );

	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );	
	topology->init();


	TopologyAssociationMapperOptions & op = o->getOptions<TopologyAssociationMapperOptions>();
	op.topology.componentPointer = topology ;
	o->init();

	string i1( "test 1" );
	string i2( "ich bin C++" );

	AssociateID aid1 = o->create_instance_mapping( i1 );
	AssociateID aid2 = o->create_instance_mapping( i2 );
	myAssert( aid1 != aid2 );

	cout << aid1 << endl;
	cout << aid2 << endl;

	try{
		o->lookup_instance_mapping( 444 );
		myAssert(false);
	}catch(NotFoundError & e){}

	AssociateID aid3 = o->create_instance_mapping( i1 );
	cout << aid3 << endl;

	myAssert( aid1 == aid3 );

	const string & ref = o->lookup_instance_mapping( aid1 );
	cout << ref << endl;

	myAssert( o->lookup_instance_mapping( aid1 ) == i1 );

	// PROCESS TEST

	ProcessID pid = {.nid = 1, .pid = 2, .time = 3};
	OntologyAttribute a1;
	a1.aID = 1;
	a1.storage_type = VariableDatatype::Type::UINT32;

	OntologyAttribute a2;
	a2.aID = 2;
	a2.storage_type = VariableDatatype::Type::UINT32;

	OntologyValue v1( (uint32_t) 34 );
	OntologyValue v2( (uint32_t) 35 );
	o->set_process_attribute( pid, a1, v1 );
	o->set_process_attribute( pid, a1, v1 );

	try {
		o->set_process_attribute( pid, a1, v2 );
		myAssert( false );
	} catch( IllegalStateError & e ) {}

	try {
		const OntologyValue & vp = o->lookup_process_attribute( pid, a2 );
		cerr << "vp = " << vp << "\n";
		myAssert( false );
	} catch( NotFoundError & e ) {}

	OntologyValue vp = o->lookup_process_attribute( pid, a1 );	

	myAssert( vp == v1 );

	// COMPONENT TEST

	ComponentID cid = {.pid = pid, .id = 4};
	cout << "Datatype: " << sizeof( cid ) << endl;

	o->set_component_attribute( cid, a1, v1 );
	o->set_component_attribute( cid, a1, v1 );

	try {
		o->set_component_attribute( cid, a1, v2 );
		myAssert( false );
	} catch( IllegalStateError & e ) {}

	try {
		vp = o->lookup_component_attribute( cid, a2 );
		myAssert( false );
	} catch( NotFoundError & e ) {}


	vp = o->lookup_component_attribute( cid, a1 );
	myAssert( vp == v1 );

	delete( o );
	delete( topology );

	cout << "OK" << endl;

	return 0;
}


