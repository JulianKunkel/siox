#include <gmodule.h>

#include <assert.h>

#include <core/module/ModuleLoader.hpp>
#include <core/component/Component.hpp>

namespace core {

	ModuleError::ModuleError( string module_path, string module_name, string interface, string msg )
	{
		this->interface = interface;
		this->msg = msg;

		if( module_path != "" )
			this->module = module + "/" + module_name + ".so";
		else
			this->module = module_name + ".so";

		this->msg = "Critical error upon loading module \"" + module + "\" with interface \"" + interface + "\". " + msg;
	}


	const char * ModuleError::what() const throw()
	{
		return this->msg.c_str();
	}

	namespace module_internal {

		void * module_create_instance( string module_path, string module_name, string interface_name ) throw( ModuleError )
		{
			// check basic consistency of the arguments
			assert( module_name != "" );
			assert( interface_name != "" );

			string fullQualifiedFileName = ( module_path != "" ) ? module_path + "/lib" + module_name + ".so" : "lib" + module_name + ".so";

			GModule * module = g_module_open( fullQualifiedFileName.c_str(), G_MODULE_BIND_LAZY );

			if( module == nullptr ) {
				const char * errmsg = g_module_error();
				throw ModuleError( module_path, module_name, interface_name, errmsg );
			}

			// lookup symbol which will create the instance

			void *( *module_instance_function )();
			string instance_fkt_name = Component::component_instanciator_name(interface_name);

			if( !g_module_symbol( module, instance_fkt_name.c_str(), ( gpointer * ) &module_instance_function ) ) {
				throw ModuleError( module_path, module_name, interface_name,
				                   "Instance function " + instance_fkt_name + "() not implemented. Are you sure this module offers the interface you are looking for?" );
			}

			// create the instance:
			void * instance = module_instance_function();

			if( instance != nullptr ) {
				return instance;
			}

			throw ModuleError( module_path, module_name, interface_name, "Instance function returned a null pointer." );
		}

	}

}

