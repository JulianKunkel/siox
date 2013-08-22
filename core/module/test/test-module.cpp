#include "test-module.hpp"

std::string HelloWorld::testfunc()
{
	return "Hello World!";
}


// module part.
extern "C" {
#include "./module.h"

	void * get_instance_hello_world()
	{
		return new HelloWorld();
	}
}
