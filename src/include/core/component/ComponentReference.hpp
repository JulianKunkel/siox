#ifndef SIOX_COMPONENT_REFERENCE_H
#define SIOX_COMPONENT_REFERENCE_H

#include <core/component/Component.hpp>

namespace core {

// For testing we may reference an object directly without casting an integer to a pointer.
#ifdef COMPONENT_NO_OBJECT_INJECTION
	// instances in options are the real objects

	typedef Component* ComponentReference;

#define GET_INSTANCE(TYPE, Y) static_cast<TYPE>(Y);

#else // OBJECT_INJECTION

	// objects are injected using the configuration file.

	typedef uint64_t ComponentReferenceID;

	/*
	 * Use this class in the options to create a reference to a required component (interface).
	 */
	//@externalserialization
	class ComponentReference {
		public:
			ComponentReferenceID componentID = 0;
			Component *  componentPointer = nullptr;
			bool global = false;

			ComponentReference(){}
			ComponentReference(Component * component) : componentPointer(component){}

			ComponentReference & operator= ( const Component * component ){
				this->componentPointer = const_cast<Component *>(component);
				return *this;
			}
	};

#define GET_INSTANCE(TYPE, Y) (dynamic_cast<TYPE *>(Y.componentPointer))

#endif // OBJECT_INJECTION

}

#endif