#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace core;

// You may or may not use this advanced options.
//@serializable
class DereferencingFacadeOptions: public ComponentOptions {
	public:
		ComponentReference topology;
		ComponentReference ontology;
		ComponentReference system_information_manager;
		ComponentReference association_mapper;
		ComponentReference reasoner;
};


#endif
