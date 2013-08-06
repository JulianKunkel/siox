#include <assert.h>

#include <core/component/ComponentRegistrar.hpp>

using namespace std;

namespace core {
	Component * ComponentRegistrar::lookup_component(ComponentReferenceID which) throw (InvalidComponentException){
		Component * c = map[which];
		if( c == nullptr){
			throw InvalidComponentException(which);
		}
		return c;
	}
	void ComponentRegistrar::register_component(ComponentReferenceID nr, Component * component){
		assert(nr != 0); // 0 is an invalid / none component
		assert(map[nr] == nullptr);
		map[nr] = component;
		list.push_back(component);
	}

	void ComponentRegistrar::shutdown(){
		  for (auto it = list.begin(); it != list.end(); it++){
		  	  	delete(*it);
		  }
	}
};
