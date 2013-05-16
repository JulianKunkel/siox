#include <iostream>
#include <mutex>

#include "ActivityMultiplexerListener_Impl1.hpp"


std::mutex mut_print;

int ActivityMultiplexerListener_Impl1::nextID = 0;

ActivityMultiplexerListener_Impl1::ActivityMultiplexerListener_Impl1() 
{
	ID = nextID;
	nextID++;
}

ActivityMultiplexerListener_Impl1::~ActivityMultiplexerListener_Impl1()
{

}

void ActivityMultiplexerListener_Impl1::print()
{
	std::cout << "Listener-" << ID;
}

void ActivityMultiplexerListener_Impl1::Notify(Activity * activity)
{	
	std::unique_lock<std::mutex> lock(mut_print);
	
	// debug	
	std::cout << "Notify("; 
	activity->print();	
	std::cout << ")";
	std::cout << " <- ";
	this->print();
	std::cout << std::endl;

	// logic not relevent as it is plugin specific
}

