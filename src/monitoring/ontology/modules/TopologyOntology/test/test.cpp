#include <assert.h>
#include <iostream>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp>

#include <monitoring/topology/Topology.hpp>
#include <monitoring/topology/RamTopologyOptions.hpp>
#include <util/Util.hpp>

using namespace std;

using namespace monitoring;
using namespace core;

int main( int argc, char const * argv[] )
{
	Ontology * o = core::module_create_instance<Ontology>( "", "siox-monitoring-TopologyOntology", ONTOLOGY_INTERFACE );

	Topology* topology = module_create_instance<Topology>( "", "siox-monitoring-RamTopology", MONITORING_TOPOLOGY_INTERFACE );
	{
	// RamTopologyOptions & o = topology->getOptions<RamTopologyOptions>();
	}
	topology->init();

	TopologyOntologyOptions * op = new TopologyOntologyOptions();
	op->topology.componentPointer = topology ;
	o->init( op );

	string domain( "test" );
	string s_a1( "a1" );
	string s_a2( "a2" );
	string s_a3( "stringAttr" );

	const OntologyAttribute & a1 = o->register_attribute( domain, s_a1, VariableDatatype::Type::UINT32 );
	assert( a1.aID != 0 );

	const OntologyAttribute & a2 = o->register_attribute( domain, s_a2, VariableDatatype::Type::UINT32 );

	const OntologyAttribute & a3 = o->register_attribute( domain, s_a1, VariableDatatype::Type::UINT32 );
	assert( a3 == a1 );
	_unused(a3);

	try {
		const OntologyAttribute & a4 = o->register_attribute( domain, s_a1, VariableDatatype::Type::UINT64 );
		cerr << "a4.aID = " << a4.aID << "\n";
		assert( false );
	} catch( IllegalStateError & e ) {
	}


	const OntologyAttribute & a5 = o->register_attribute( domain, s_a3, VariableDatatype::Type::STRING );

	cout << "ID1: " << a1.aID << endl;
	cout << "ID2: " << a2.aID << endl;


	OntologyAttribute a4 = o->lookup_attribute_by_name( domain, s_a1 );
	assert( a4 == a1 );

	OntologyAttributeID aid = a1.aID;

	a4 = o->lookup_attribute_by_ID( aid );
	assert( a4 == a1 );

	OntologyValue val( ( uint32_t ) 35 );
	OntologyValue vText( "test&my" );
	OntologyValue vText2( "test&my" );
	assert( vText == vText2 );

	o->attribute_set_meta_attribute( a1, a2, val );
	o->attribute_set_meta_attribute( a1, a2, val );
	o->attribute_set_meta_attribute( a1, a5, vText );


	OntologyValue val2( ( uint32_t ) 36 );
	try {
		o->attribute_set_meta_attribute( a1, a2, val2 );
		assert( false );
	} catch( IllegalStateError & e ) {
	}

	assert( o->enumerate_meta_attributes( a1 ).size() == 2 );

	const OntologyValue & ret = o->lookup_meta_attribute( a1, a2 );
	cout << "Type: " << ( int ) ret.type() << " " << endl;
	cout << "Val: " << ret.uint32() << endl;

	delete( o );
	delete( topology );

	cout << "OK" << endl;

	return 0;
}


