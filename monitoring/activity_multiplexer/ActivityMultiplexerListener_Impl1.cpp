#include <iostream>
#include <mutex>

#include "ActivityMultiplexerListener_Impl1.hpp"


std::mutex mut_print;

/**
 * Listener Sync Impl1
 */
int ActivityMultiplexerListenerSync_Impl1::nextID = 0;

ActivityMultiplexerListenerSync_Impl1::ActivityMultiplexerListenerSync_Impl1() 
{
	ID = nextID;
	nextID++;
}

ActivityMultiplexerListenerSync_Impl1::~ActivityMultiplexerListenerSync_Impl1()
{

}

void ActivityMultiplexerListenerSync_Impl1::print()
{
	std::cout << "SyncListener-" << ID;
}

void ActivityMultiplexerListenerSync_Impl1::Notify(Activity * activity)
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


/**
 * Listener Async Impl1
 */
int ActivityMultiplexerListenerAsync_Impl1::nextID = 0;

ActivityMultiplexerListenerAsync_Impl1::ActivityMultiplexerListenerAsync_Impl1() 
{
	ID = nextID;
	nextID++;
}

ActivityMultiplexerListenerAsync_Impl1::~ActivityMultiplexerListenerAsync_Impl1()
{

}

void ActivityMultiplexerListenerAsync_Impl1::print()
{
	std::cout << "AsyncListener-" << ID;
}


void ActivityMultiplexerListenerAsync_Impl1::Notify(Activity * activity)
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


void ActivityMultiplexerListenerAsync_Impl1::Reset(int dropped)
{
	// handle..
}
