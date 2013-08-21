#ifndef SIOX_COMPONENTREGISTRAR_H
#define SIOX_COMPONENTREGISTRAR_H

#include <map>
#include <list>
#include <exception>
#include <sstream>

#include <core/component/ComponentReference.hpp>
#include <core/component/Component.hpp>

namespace core {


class InvalidComponentException: public std::exception{
public:

  InvalidComponentException(ComponentReferenceID invalid){
  	invalidNr = invalid;
  }

  virtual const char* what() const throw()
  {
  	return "The component has not been registered in the ModuleRegistrar";
  }

  ComponentReferenceID invalidNr;
};


class ComponentRegistrar{
public:
	Component * lookup_component(ComponentReferenceID which) throw (InvalidComponentException);
	void register_component(ComponentReferenceID nr, Component * component);

	// do not tamper with the list, otherwise you get what you deserve...
	std::list<Component*> & list_registered_components(){
		return list;
	}

	const int number_of_registered_components(){
		return list.size();
	}

	/*
	 * Free all allocated components and call their shutdown() method.
	 */
	void shutdown();

private:
	std::map<ComponentReferenceID, Component*> map;
	std::list<Component*> list;
};

}

#endif 