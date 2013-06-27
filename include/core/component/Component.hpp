#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <string>

#include <core/container/container.hpp>

namespace core {

typedef Container ComponentOptions;

class Component {
public:
	// The init method uses the configuration options to configure the component.
	// It is responsible to delete the options if the options are not relevant any more after the initalization.
	virtual void init(ComponentOptions * options) = 0;
	virtual ComponentOptions * get_options() = 0;
	virtual void shutdown() = 0;

	virtual ~Component(){}
};

}

#endif
