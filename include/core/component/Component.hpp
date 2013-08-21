#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <string>
#include <stdint.h>
#include <assert.h>

#include <core/container/container.hpp>

namespace core {

typedef Container ComponentOptions;

#ifdef NO_OBJECT_INJECTION
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

class Component {
private:
	ComponentOptions * options = nullptr;
protected:

	// This function returns the available options of this component.
	virtual ComponentOptions * AvailableOptions() = 0;
public:
	#define INSTANCE_FKT_PREFIX "get_instance_"
	#define COMPONENT_INSTANCIATOR_NAME(INTERFACE_NAME) get_instance_ ## INTERFACE_NAME
	// The init method uses the configuration options to configure the component.
	// It is responsible to delete the options if the options are not relevant any more after the initalization.

	// Call this function to initialize the object with the current options
	virtual void init() = 0;

	// Call this function to initialize the object with given options
	// Ownership of the options is given to the Component!
	void init(ComponentOptions * options){
		assert(this->options == nullptr ); // otherwise it has been initialized.
		// if the options have been fetched using getOptions* then only init() must be used.

		this->options = options;
		init();
	}

	inline ComponentOptions & getOptions(){
		if( options == nullptr){
			options = AvailableOptions();
			assert(options != nullptr);
		}
		return *options;
	}

	template<class TYPE>
	inline TYPE & getOptions(){
		if( options == nullptr){
			options = AvailableOptions();
			assert(options != nullptr);
		}
		return *dynamic_cast<TYPE*>(options);
	}

	//virtual void shutdown() = 0; // Shutdown is merged into the destructor.

	virtual ~Component(){ 
		if(options != nullptr) 
			delete(options);
	}
};

}

#endif
