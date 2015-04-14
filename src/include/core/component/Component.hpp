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

			bool started = false;
		public:
			// The init method uses the configuration options to configure the component.
			// It is responsible to delete the options if the options are not relevant any more after the initalization.

			// Call this function to initialize the object with the current options
			virtual void init() = 0;

			// Call this function to initialize the object with given options
			// Ownership of the options is given to the Component!
			void setOptions( ComponentOptions * options ) {
				if( this->options != nullptr ){
					// it has been initialized.
					delete this->options;
				} 
				// if the options have been fetched using getOptions* then only init() must be used.
				this->options = options;
			}

			// This function will (and must) be called before the destructor of the object is called.
			virtual void finalize(){

			}

			// stop processing of threads
			virtual void stop() {
				started = false; 
			}

			// if this module is mandatory, then it will only be stopped for fork()
			virtual bool isMandatoryModule(){ 
				return false;
			}

			// start processing of threads
			virtual void start() {
				started = true;
			}

			bool isStarted() const{
				return started;
			}

			inline ComponentOptions & getOptions() {
				if( this->options == nullptr ) {
					this->options = AvailableOptions();
					assert( this->options != nullptr );
				}
				return * this->options;
			}

			template<class TYPE>
			inline TYPE & getOptions() {
				if( options == nullptr ) {
					options = AvailableOptions();
					assert( options != nullptr );
				}
				return *dynamic_cast<TYPE *>( options );
			}

			virtual ~Component() {
				if( options != nullptr ){
					delete( options );
				}
			}
	};

}

#endif
