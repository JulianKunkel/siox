#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <string>
#include <stdint.h>
#include <assert.h>

#include <core/container/container.hpp>
#include <core/module/ModuleInterface.hpp>

namespace core {

	typedef Container ComponentOptions;

	class Component : public ModuleInterface {
		private:
			ComponentOptions * options = nullptr;
		protected:

			// This function returns the available options of this component.
			virtual ComponentOptions * AvailableOptions() = 0;
		public:
			// Compile time and run time calculation of the module instanciator symbol names.
			#define COMPONENT_INSTANCIATOR_NAME(INTERFACE_NAME) get_instance_ ## INTERFACE_NAME
			static std::string component_instanciator_name(const std::string &interface_name) throw() {
				return "get_instance_" + interface_name;
			};

			// The init method uses the configuration options to configure the component.
			// It is responsible to delete the options if the options are not relevant any more after the initalization.

			// Call this function to initialize the object with the current options
			virtual void init() = 0;

			// Call this function to initialize the object with given options
			// Ownership of the options is given to the Component!
			void init( ComponentOptions * options ) {
				assert( this->options == nullptr ); // otherwise it has been initialized.
				// if the options have been fetched using getOptions* then only init() must be used.

				this->options = options;
				init();
			}

			inline ComponentOptions & getOptions() {
				if( options == nullptr ) {
					options = AvailableOptions();
					assert( options != nullptr );
				}
				return *options;
			}

			template<class TYPE>
			inline TYPE & getOptions() {
				if( options == nullptr ) {
					options = AvailableOptions();
					assert( options != nullptr );
				}
				return *dynamic_cast<TYPE *>( options );
			}

			//virtual void shutdown() = 0; // Shutdown is merged into the destructor.

			virtual ~Component() {
				if( options != nullptr )
					delete( options );
			}
	};

}

#endif
