#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <core/container/container.hpp>

namespace core {

class ComponentOptions : Container{
};

class Component {
public:
	virtual void init(ComponentOptions * options) = 0;
	virtual ComponentOptions * get_options() = 0;
	virtual void shutdown() = 0;
};

}

#endif
