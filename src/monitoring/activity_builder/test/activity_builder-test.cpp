/*
 * File:   test.cpp
 * Author: mickler
 *
 * Created on July 22, 2013, 2:58 PM
 */

#include "monitoring/activity_builder/ActivityBuilder.hpp"
#include "monitoring/ontology/Ontology.hpp"

#include <cstdlib>

using namespace std;
using namespace monitoring;

static void test_single_activity( ActivityBuilder * ab, ComponentID & cid, UniqueComponentActivityID & ucaid )
{
	Activity * a;
	Attribute attr, attr2;

	a = ab->startActivity( cid, ucaid, nullptr );
	ab->stopActivity( a, nullptr );

	attr.id = 1;
	attr.value = 3.2;
	ab->setActivityAttribute( a, attr );
	attr2.id = 3;
	attr2.value = 0.1;
	ab->setActivityAttribute( a, attr2 );

	ab->endActivity( a );
	a->print();
}

static void test_remote_call( ActivityBuilder * ab, ComponentID & cid, UniqueComponentActivityID & ucaid )
{
	Activity * a;
	RemoteCall * rc;
	Attribute attr, attr2;

	a = ab->startActivity( cid, ucaid, nullptr );
	rc = ab->setupRemoteCall( a, 1, 1, 1 );

	attr.id = 13;
	attr.value = 1.1;
	ab->setRemoteCallAttribute( rc, attr );
	attr2.id = 14;
	attr2.value = -1.1;
	ab->setRemoteCallAttribute( rc, attr2 );
	ab->startRemoteCall( rc, nullptr );

	ab->stopActivity( a, nullptr );
	ab->endActivity( a );
	a->print();
}

static void test_remote_activity( ActivityBuilder * ab, ComponentID & cid, UniqueComponentActivityID & ucaid )
{
	Activity * a;
	Attribute attr, attr2;

	a = ab->startActivity( cid, ucaid, 1, 1, 1, nullptr );

	attr.id = 30;
	attr.value = 31.1;
	ab->setActivityAttribute( a, attr );
	attr2.id = 31;
	attr2.value = -31.1;
	ab->setActivityAttribute( a, attr2 );

	ab->stopActivity( a, nullptr );
	ab->endActivity( a );
	a->print();
}

/*
 *
 */
int main( int argc, char ** argv )
{
	ComponentID cid;
	UniqueComponentActivityID ucaid;
	ActivityBuilder * ab;

	cid.pid.nid = 1;
	cid.pid.pid = 1;
	cid.pid.time = 234;
	cid.id = 0;
	ucaid = 7;

	ab = ActivityBuilder::getThreadInstance();

	test_single_activity( ab, cid, ucaid );
	test_remote_activity( ab, cid, ucaid );
	test_remote_call( ab, cid, ucaid );

	delete ab;

	return 0;
}

