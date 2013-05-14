#include <iostream>
#include "ActivityMultiplexerListener_Impl1.hpp"

int ActivityMultiplexerListener_Impl1::nextLID = 0;

ActivityMultiplexerListener_Impl1::ActivityMultiplexerListener_Impl1() 
{
	LID = nextLID;
	nextLID++;
}

ActivityMultiplexerListener_Impl1::~ActivityMultiplexerListener_Impl1()
{

}

void ActivityMultiplexerListener_Impl1::print()
{
	std::cout << "Listener-" << LID;
}

void ActivityMultiplexerListener_Impl1::Notify(Activity * activity)
{
	// debug
	this->print();
	std::cout << " -> Notify(";
	activity->print();
	std::cout << ")" <<  std::endl;

	// logic not relevent as it is plugin specific
}

