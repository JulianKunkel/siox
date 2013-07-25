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

/*
 * 
 */
int main(int argc, char** argv)
{
	ComponentID cid;
	UniqueComponentActivityID caid;
	Activity* a;
	Attribute attr, attr2;
	ActivityBuilder* ab;

	cid.pid.nid = 1;
	cid.pid.pid = 1;
	cid.pid.time = 234;
	cid.num = 0;
	caid = 7;

	ab = ActivityBuilder::getInstance();
	a = ab->startActivity( &cid, &caid, NULL);
	ab->stopActivity(a, NULL);

	attr.id = 1;
	attr.value = 3.2;
	ab->addActivityAttribute(a, &attr);
	attr2.id = 3;
	attr2.value = 0.1;
	ab->addActivityAttribute(a, &attr2);

	ab->endActivity(a);
	a->print();
	return 0;
}

