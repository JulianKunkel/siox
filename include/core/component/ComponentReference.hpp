#ifndef SIOX_COMPONENT_REFERENCE_H
#define SIOX_COMPONENT_REFERENCE_H

namespace core {

// For testing we may reference an object directly without casting an integer to a pointer.
#ifdef COMPONENT_NO_OBJECT_INJECTION
	// instances in options are the real objects

	typedef void * instance ComponentReference;

	#define GET_INSTANCE(TYPE, Y) static_cast<TYPE>(Y);

#else // OBJECT_INJECTION

	// objects are injected using the configuration file.

	typedef uint64_t ComponentReferenceID;

	/*
	 * Use this class in the options to create a reference to a required component (interface).
	 */
	class ComponentReference{
	public:
		ComponentReferenceID componentID = 0;
		bool global = false;

		template<class TYPE>
		TYPE* instance(){
			return (TYPE*)(componentID);
		}
	};

	#define GET_INSTANCE(TYPE, Y) Y.instance<TYPE>();

#endif // OBJECT_INJECTION

}

#endif