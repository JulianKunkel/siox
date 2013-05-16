#include <iostream>
#include "Activity.hpp"

int Activity::nextAID = 0;

Activity::Activity()
{
	AID = nextAID;
	nextAID++;
}

Activity::~Activity()
{

}

void Activity::print() {
	std::cout << "Activity-" << AID;	
}
