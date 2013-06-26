#include <iostream>

#include "ActivityMultiplexer_Impl1.hpp"

using namespace std;

namespace monitoring{


void ActivityMultiplexer_Impl1::Log(Activity * element) 
{
	// readers and adding are alike (valid iterator), share ability to dec/inc
	{
		std::lock_guard<std::mutex> lock(inc);
		not_invalidating++;
	}

	for (auto listener=listeners.begin(); listener!=listeners.end(); ++listener) {
		(*listener)->Notify(element);
	}

	{
		std::lock_guard<std::mutex> lock(dec);
		not_invalidating--;
	}
}

void ActivityMultiplexer_Impl1::registerListener(MultiplexerListener<Activity> * listener)
{
	// readers and adding are alike (valid iterator), share ability to dec/inc
	{
		std::lock_guard<std::mutex> lock(inc);
		not_invalidating++;
	}

	listeners.push_back(listener);	
	
	{
		std::lock_guard<std::mutex> lock(dec);
		not_invalidating--;
	}
}

void ActivityMultiplexer_Impl1::unregisterListener(MultiplexerListener<Activity> * listener)
{
	// exclusive, as removing may invalidate iterator
	std::lock_guard<std::mutex> lock(inc);
	while( not_invalidating != 0 ) {
		// wait, candidate for condition vairable?
	}
	listeners.remove(listener);
}

}

extern "C"{
#include <monitoring/activity_multiplexer/module.h>

void * get_instance_monitoring_activitymultiplexer(){
	return new monitoring::ActivityMultiplexer_Impl1();
}
}


