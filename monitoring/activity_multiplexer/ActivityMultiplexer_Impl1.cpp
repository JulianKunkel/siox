#include <iostream>

#include "ActivityMultiplexer_Impl1.hpp"

namespace monitoring{

void ActivityMultiplexer_Impl1::Log(Activity * element) {
	std::cout << "Log()\n";
}



}


extern "C"{
#include <monitoring/activity_multiplexer/module.h>

void * get_instance_monitoring_activitymultiplexer(){
	return new monitoring::ActivityMultiplexer_Impl1();
}
}


