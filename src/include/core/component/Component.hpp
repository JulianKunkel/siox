#ifndef SIOX_COMPONENT_H
#define SIOX_COMPONENT_H

#include <string>
#include <stdint.h>
#include <assert.h>

#include <core/component/component-options.hpp>
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

			//TODO The return type of this template breaks with the convention that references must not be NULL. Change this to return a pointer.
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
