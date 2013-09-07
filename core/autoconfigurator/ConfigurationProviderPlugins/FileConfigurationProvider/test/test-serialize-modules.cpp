#include "test-serialize-modules.hpp"

using namespace core;


void MyChildModule::init()
{
}

ComponentOptions * MyChildModule::AvailableOptions()
{
	return new MyChildModuleOptions();
}

void MyParentModule::init()
{
}

ComponentOptions * MyParentModule::AvailableOptions()
{
	return new MyParentModuleOptions();
}

extern "C" {
	void * get_instance_parent()
	{
		return new MyParentModule();
	}
}


extern "C" {
	void * get_instance_child()
	{
		return new MyChildModule();
	}
}
