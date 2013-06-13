#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <list>
#include <string>
#include <typeinfo>

using namespace std;

namespace core {

class ComponentOptionEntry{
public:
	string name;
	type_info typ;
	void * address;

	ComponentOptionEntry(string name, type_info typ, void * address) {
		this.name = name;
		this.typ = typ;
		this.address = address;
	}
};

class ComponentOptions {
public:
	virtual void get_component_options(list<ComponentOptionEntry> & out_params);
};

class Component {
	void init(ComponentOptions * options);
	ComponentOptions * get_options();
	void shutdown();
};

}

#endif
