#ifndef MODULE_AUTOCONFIGURATOR_HPP
#define MODULE_AUTOCONFIGURATOR_HPP

#include <stdint.h>
#include <string>

#include <core/component/component-options.hpp>

using namespace core;

//@serializable
class LoadModule : public Container {
	public:
		// component number, this is useful for references
		uint32_t componentID;
		// name of the module
		std::string name;
		// path to search for the module
		std::string path;
		// required interface
		std::string interface;
};

#endif