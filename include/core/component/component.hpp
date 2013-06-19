#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <core/container/container.hpp>

namespace core {

class ComponentOptions : Container{
};

class Component {
	void init(ComponentOptions * options);
	ComponentOptions * get_options();
	void shutdown();
};

}

#endif
