#include <iostream>

#include "ActivityMultiplexer_Impl1.hpp"

using namespace std;

namespace monitoring{


void ActivityMultiplexer_Impl1::Log(Activity * element) 
{
	list<MultiplexerListener<Activity> *>::const_iterator listener;	
	for ( listener=listeners.begin(); listener!=listeners.end(); ++listener) {
		(*listener)->Notify(element);
	}
}

void ActivityMultiplexer_Impl1::registerListener(MultiplexerListener<Activity> * listener)
{
	listeners.push_back(listener);		
}

void ActivityMultiplexer_Impl1::unregisterListener(MultiplexerListener<Activity> * listener)
{
	listeners.remove(listener);
}


} // monitoring


extern "C"{
#include <monitoring/activity_multiplexer/module.h>

void * get_instance_monitoring_activitymultiplexer(){
	return new monitoring::ActivityMultiplexer_Impl1();
}
}


