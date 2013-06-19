#ifndef SIOX_MODULE_LOADER_H
#define SIOX_MODULE_LOADER_H

#include <exception>
#include <string>


using namespace std;


namespace core{

/** 
* The module loader interface provides a simple abstraction to "plugin"  component modules at runtime.
* Load a single module and extracts any numbers of components with different interfaces from it.
*/
 
/**
 * Exception thrown upon an error of the module instantiation.
 */
class ModuleError: public std::exception{
	private:
		string interface;
		string module;
		string msg;
	public:
		ModuleError(string module_path, string module_name, string interface, string msg);
		 const char* what() const throw();
};

namespace module_internal{
	// this definition reduces the definition required in the header file significantly.
	void * module_create_instance(string module_path, string module_name, string interface_name) throw (ModuleError);
}

/** 
 * This function loads the given module, localizes the interface as specified in the name.
 */
template <class COMPONENTTYPE> 
COMPONENTTYPE * module_create_instance(string module_path, string module_name, string interface_name) throw (ModuleError){
	void * instance =  module_internal::module_create_instance(module_path, module_name, interface_name);
	COMPONENTTYPE * instance_casted = static_cast<COMPONENTTYPE*>(instance);
	if (instance_casted == nullptr){
		throw ModuleError(module_path, module_name, interface_name, "Instance has an invalid type. Fatal module error!");
		// WARNING module instance may now leak memory, but this is an implementation error of the module which must be fixed there.
	}
	return instance_casted;
}

// COMPONENTTYPE is not restricted to component but it could be
// #include <core/component/component.hpp>
//#include <boost/type_traits.hpp>
//#include <boost/static_assert.hpp>	
//BOOST_STATIC_ASSERT(boost::is_base_of<core::component, COMPONENTTYPE>::value); 

} // end namespace

#endif 
