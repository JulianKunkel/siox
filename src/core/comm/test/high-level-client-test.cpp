#include <assert.h>
#include <iostream>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>
#include <util/JobProcessor.hpp>

using namespace core;
using namespace std;


int main(){
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	delete(comm);
}