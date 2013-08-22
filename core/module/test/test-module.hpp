#ifndef SIOX_TEST_MODULE_H
#define SIOX_TEST_MODULE_H

#include <string>

#include <core/module/ModuleInterface.hpp>

class HelloWorld : public core::ModuleInterface {
	public:
		virtual std::string testfunc() = 0;
		virtual ~HelloWorld(){};
};

class HelloSaturn : public core::ModuleInterface {
	public:
		virtual std::string alternativeFunc() = 0;
		virtual ~HelloSaturn(){};
};


#endif

