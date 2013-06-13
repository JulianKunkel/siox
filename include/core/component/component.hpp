#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <list>

using namespace std;

namespace core{

class ComponentOptionEntry{
public:
	class<?> typ;
	void * address;
}

class ComponentOptions{
public:
	virtual map<string, ComponentOptionEntry> * get_component_options();
}

class Component{
	void init(ComponentOptions * options);
	ComponentOptions * get_options();
	void shutdown();
};

}

#endif
