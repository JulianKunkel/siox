#ifndef SIOX_TEST_MODULE_H
#define SIOX_TEST_MODULE_H

#include <string>

#include <core/module/ModuleInterface.hpp>

class HelloWorld : public core::ModuleInterface{
	public:
		virtual std::string testfunc();
};

#endif

