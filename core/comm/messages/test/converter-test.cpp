#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <string>

#include <siox.pb.h>
#include <core/comm/messages/BufferConverter.hpp>

BOOST_AUTO_TEST_CASE( attribute_value_test )
{
	BufferConverter c;

	const int64_t int64 = 64;
	const VariableDatatype d64( int64 );

	BOOST_CHECK_EQUAL( int64, d64.int64() );

	buffers::AttributeValue * b64 = new buffers::AttributeValue();
	c.attribute_value( d64, *b64 );

	BOOST_CHECK_EQUAL( d64.int64(), b64->i64() );

	const int32_t int32 = 32;
	const VariableDatatype d32( int32 );

	BOOST_CHECK_EQUAL( int32, d32.int32() );

	buffers::AttributeValue * b32 = new buffers::AttributeValue();
	c.attribute_value( d32, *b32 );

	BOOST_CHECK_EQUAL( d32.int32(), b32->i32() );

	const uint64_t uint64 = 64;
	const VariableDatatype du64( uint64 );

	BOOST_CHECK_EQUAL( uint64, du64.uint64() );

	buffers::AttributeValue * bu64 = new buffers::AttributeValue();
	c.attribute_value( du64, *bu64 );

	BOOST_CHECK_EQUAL( du64.uint64(), bu64->ui64() );

	const float f = 0.11;
	const VariableDatatype df( f );

	BOOST_CHECK_EQUAL( f, df.flt() );

	buffers::AttributeValue * bf = new buffers::AttributeValue();
	c.attribute_value( df, *bf );

	BOOST_CHECK_EQUAL( df.flt(), bf->f() );

	const double d = 0.22;
	const VariableDatatype dd( d );

	BOOST_CHECK_EQUAL( d, dd.dbl() );

	buffers::AttributeValue * bd = new buffers::AttributeValue();
	c.attribute_value( dd, *bd );

	BOOST_CHECK_EQUAL( dd.dbl(), bd->d() );

	const std::string str = "hello siox!";
	const VariableDatatype ds( str );

	BOOST_CHECK_EQUAL( str, ds.str() );

	buffers::AttributeValue * bs = new buffers::AttributeValue();
	buffers::AttributeValue & bss = *bs;

	c.attribute_value( ds, bss );

	BOOST_CHECK_EQUAL( ds.str(), bss.str() );

}


BOOST_AUTO_TEST_CASE( attribute_test )
{
	BufferConverter c;

	const std::string str = "it's me again!";
	const AttributeValue value( str );
	const OntologyAttributeID id = 100;

	const Attribute attr( id, value );

	BOOST_CHECK_EQUAL( id, attr.id );
	BOOST_CHECK_EQUAL( value, attr.value );

	buffers::Attribute battr;
	c.attribute( attr, battr );

	BOOST_CHECK_EQUAL( attr.id, battr.id() );
	BOOST_CHECK_EQUAL( value.str(), battr.value().str() );

	const uint64_t i64 = 42;
	const AttributeValue value2( i64 );
	const OntologyAttributeID id2 = 100;

	const Attribute attr2( id2, value2 );

	BOOST_CHECK_EQUAL( id2, attr2.id );
	BOOST_CHECK_EQUAL( value2, attr2.value );

	buffers::Attribute battr2;
	c.attribute( attr2, battr2 );

	BOOST_CHECK_EQUAL( attr2.id, battr2.id() );
	BOOST_CHECK_EQUAL( value2.uint64(), battr2.value().ui64() );

}


BOOST_AUTO_TEST_CASE( remote_call_id_test )
{
	BufferConverter c;

	const NodeID nid = 100;
	const UniqueComponentActivityID uuid = 200;
	const AssociateID instance = 300;
	const RemoteCallIdentifier rc( nid, uuid, instance );

	BOOST_CHECK_EQUAL( nid, rc.nid );
	BOOST_CHECK_EQUAL( uuid, rc.uuid );
	BOOST_CHECK_EQUAL( instance, rc.instance );

	buffers::RemoteCallIdentifier brc;
	c.remote_call_id( rc, brc );

	BOOST_CHECK_EQUAL( nid, brc.nid() );
	BOOST_CHECK_EQUAL( uuid, brc.uuid() );
	BOOST_CHECK_EQUAL( instance, brc.instance() );
}


BOOST_AUTO_TEST_CASE( remote_call_test )
{
	BufferConverter c;

	const NodeID nid = 10;
	const UniqueComponentActivityID uuid = 20;
	const AssociateID instance = 30;
	const RemoteCallIdentifier target( nid, uuid, instance );

	vector<Attribute> attributes;
	Activity * act = NULL;

	const std::string str = "it's me again!";
	const AttributeValue value( str );
	const OntologyAttributeID id = 100;
	const Attribute attr( id, value );
	attributes.push_back( attr );

	const uint64_t i64 = 42;
	const AttributeValue value2( i64 );
	const OntologyAttributeID id2 = 100;
	const Attribute attr2( id2, value2 );
	attributes.push_back( attr2 );

	const RemoteCall rc = { target, attributes, act };

	buffers::RemoteCall brc;
	c.remote_call( rc, brc );

	BOOST_CHECK_EQUAL( rc.target.nid, brc.target().nid() );
	BOOST_CHECK_EQUAL( rc.target.uuid, brc.target().uuid() );
	BOOST_CHECK_EQUAL( rc.target.instance, brc.target().instance() );

	if( rc.attributes.size() != brc.attributes_size() )
		BOOST_FAIL( "Different attribute count." );

	vector<Attribute>::const_iterator i;
	for( i = rc.attributes.begin(); i < rc.attributes.end(); ++i ) {

		buffers::Attribute a1;
		c.attribute( *i, a1 );

		bool found = false;

		for( int j = 0; j < brc.attributes_size();  j++ ) {

			const buffers::Attribute & a2 = brc.attributes( j );
			if( a1.DebugString() == a2.DebugString() ) {

				found = true;
				break;

			}

		}

		if( !found ) {
			BOOST_FAIL( "Attribute not found." );
		}

	}
}


BOOST_AUTO_TEST_CASE( activityid_test )
{
	BufferConverter c;

	const uint32_t nid = 10, pid = 20, time = 30, aid = 40;
	const uint16_t cid = 50;
	const ProcessID proc = { nid, pid, time };
	const ComponentID comp = { proc, cid };
	const ActivityID act = { comp, aid };

	BOOST_CHECK_EQUAL( nid, proc.nid );
	BOOST_CHECK_EQUAL( pid, proc.pid );
	BOOST_CHECK_EQUAL( time, proc.time );
	BOOST_CHECK_EQUAL( comp.pid, proc );
	BOOST_CHECK_EQUAL( act.cid, comp );
	BOOST_CHECK_EQUAL( act.id, aid );

	buffers::ActivityID bact;
	c.aid( act, bact );

	BOOST_CHECK_EQUAL( act.id, bact.id() );
	BOOST_CHECK_EQUAL( act.cid.id, bact.cid().id() );
	BOOST_CHECK_EQUAL( act.cid.pid.nid, bact.cid().pid().nid() );
	BOOST_CHECK_EQUAL( act.cid.pid.pid, bact.cid().pid().pid() );
	BOOST_CHECK_EQUAL( act.cid.pid.time, bact.cid().pid().time() );

}


BOOST_AUTO_TEST_CASE( activity_test )
{
	BufferConverter c;

	UniqueComponentActivityID ucaid = 10;
	Timestamp time_start = 20, time_end = 30;
	uint32_t nid = 40, pid = 50, time = 60, aid = 70;
	uint16_t cid = 80;
	ProcessID proc = { nid, pid, time };
	ComponentID comp = { proc, cid };
	ActivityID actid = { comp, aid };
	ActivityError error = 404;
	RemoteCallIdentifier * remote_invoker = NULL;

	ProcessID proc2 = { 110, 210, 310 };
	ComponentID comp2 = { proc2, 410 };
	ActivityID actid2 = { comp2, 510 };

	ProcessID proc3 = { 120, 220, 320 };
	ComponentID comp3 = { proc3, 420 };
	ActivityID actid3 = { comp3, 520 };

	vector<ActivityID> parents;
	parents.push_back( actid2 );
	parents.push_back( actid3 );

	const std::string str1 = "klingon functions don't have parameters, they have arguments, and always win them!";
	const AttributeValue value1( str1 );
	const Attribute attr1( 100, value1 );

	const AttributeValue value2( 42 );
	const Attribute attr2( 101, value2 );

	vector<Attribute> attributes;
	attributes.push_back( attr1 );
	attributes.push_back( attr2 );

	const RemoteCallIdentifier target1( 5, 10, 15 );
	const RemoteCallIdentifier target2( 10, 20, 30 );

	const std::string str3 = "all your base are belong to us";
	const AttributeValue value3( str3 );
	const Attribute attr3( 100, value3 );

	const AttributeValue value4( 42 );
	const Attribute attr4( 103, value4 );
	attributes.push_back( attr4 );

	vector<Attribute> attributes2;
	vector<Attribute> attributes3;
	attributes2.push_back( attr3 );
	attributes2.push_back( attr4 );

	RemoteCall rc1 = { target1, attributes2, NULL };
	RemoteCall rc2 = { target1, attributes3, NULL };

	vector<RemoteCall> remote_calls;
	remote_calls.push_back( rc1 );
	remote_calls.push_back( rc2 );

	Activity act( ucaid, time_start, time_end, actid, parents,
	              attributes, remote_calls, remote_invoker, error );

	buffers::Activity bact;

	c.activity( act, bact );

	BOOST_CHECK_EQUAL( act.ucaid(), bact.ucaid() );
	BOOST_CHECK_EQUAL( act.time_start(), bact.time_start() );
	BOOST_CHECK_EQUAL( act.time_stop(), bact.time_stop() );
	BOOST_CHECK_EQUAL( act.errorValue(), bact.error_value() );

	BOOST_CHECK_EQUAL( act.aid().id, bact.aid().id() );
	BOOST_CHECK_EQUAL( act.aid().cid.id, bact.aid().cid().id() );
	BOOST_CHECK_EQUAL( act.aid().cid.pid.nid, bact.aid().cid().pid().nid() );
	BOOST_CHECK_EQUAL( act.aid().cid.pid.pid, bact.aid().cid().pid().pid() );
	BOOST_CHECK_EQUAL( act.aid().cid.pid.time, bact.aid().cid().pid().time() );

	if( act.parentArray().size() != bact.parents_size() )
		BOOST_FAIL( "Different parent count." );

	for( vector<ActivityID>::const_iterator i = act.parentArray().begin();
	     i < act.parentArray().end(); i++ ) {

		buffers::ActivityID aid1;
		c.aid( *i, aid1 );

		bool found = false;

		for( int j = 0; j < bact.parents_size(); j++ ) {

			const buffers::ActivityID & aid2 = bact.parents( j );
			if( aid1.DebugString() == aid2.DebugString() ) {

				found = true;
				break;

			}

		}

		if( !found ) {
			BOOST_FAIL( "Attribute not found." );
		}

	}

	if( act.attributeArray().size() != bact.attributes_size() )
		BOOST_FAIL( "Different attribute count." );

	for( vector<Attribute>::const_iterator i = act.attributeArray().begin();
	     i < act.attributeArray().end(); i++ ) {

		buffers::Attribute attr1;
		c.attribute( *i, attr1 );

		bool found = false;

		for( int j = 0; j < bact.attributes_size();  j++ ) {

			const buffers::Attribute & attr2 = bact.attributes( j );
			if( attr1.DebugString() == attr2.DebugString() ) {

				found = true;
				break;

			}

		}

		if( !found ) {
			BOOST_FAIL( "Attribute not found." );
		}

	}

	if( act.remoteCallsArray().size() != bact.remote_calls_size() )
		BOOST_FAIL( "Different remote call count." );

	for( vector<RemoteCall>::const_iterator i = act.remoteCallsArray().begin();
	     i < act.remoteCallsArray().end(); i++ ) {

		buffers::RemoteCall rc1;
		c.remote_call( *i, rc1 );

		bool found = false;

		for( int j = 0; j < bact.remote_calls_size();  j++ ) {

			const buffers::RemoteCall & rc2 = bact.remote_calls( j );
			if( rc1.DebugString() == rc2.DebugString() ) {

				found = true;
				break;

			}

		}

		if( !found ) {
			BOOST_FAIL( "Attribute not found." );
		}

	}
}
