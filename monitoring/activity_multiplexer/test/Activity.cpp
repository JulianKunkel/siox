#include <iostream>

#include "Activity.hpp"


int Activity::nextID = 0;

Activity::Activity()
{
	ID = nextID;
	nextID++;
}

Activity::~Activity()
{

}

void Activity::print() {
	std::cout << "Activity-" << ID;	
}
