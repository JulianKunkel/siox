#ifndef SIOX_COMPONENTREGISTRAR_H
#define SIOX_COMPONENTREGISTRAR_H

#include <map>
#include <list>
#include <string>
#include <exception>
#include <sstream>
#include <utility>

#include <core/component/ComponentReference.hpp>
#include <core/component/Component.hpp>

namespace core {


	class InvalidComponentException: public std::exception {
		public:

			InvalidComponentException( ComponentReferenceID invalid ) {
				invalidNr = invalid;
			}

			virtual const char * what() const throw() {
				return "The component has not been registered in the ModuleRegistrar";
			}

			ComponentReferenceID invalidNr;
	};

	struct RegisteredComponent{
		const ComponentReferenceID id;
		const std::string section;
		const std::string moduleName;
		const Component * component;		

		RegisteredComponent( ComponentReferenceID nr, const std::string & section, const std::string & moduleName, Component * component ) : id(nr), section(section), moduleName(moduleName), component(component) {}

		std::string toString(){
			char buff[20];
			sprintf(buff, "%d", (int) id);
			return std::string("<") + buff + ">:" + moduleName;
		}
	};


	class ComponentRegistrar {
		public:
			Component * lookupComponent( ComponentReferenceID which ) throw( InvalidComponentException );
			void registerComponent( ComponentReferenceID nr, const std::string & section, const std::string & moduleName, Component * component );
			void unregisterComponent( ComponentReferenceID nr );


			// do not tamper with the list, otherwise you get what you deserve...
			std::list<RegisteredComponent *> & listRegisteredComponents() {
				return list;
			}

			template<class TYPE>
			std::list<std::pair<TYPE *, RegisteredComponent*>> listAllComponentsOfATypeFull( ) {
				std::list<std::pair<TYPE *, RegisteredComponent*> > lst;

				std::list<RegisteredComponent *> & vector = listRegisteredComponents();

				for( auto itr = vector.begin(); itr != vector.end(); itr++ ) {
					TYPE * ret = dynamic_cast<TYPE *>( const_cast<Component*>((*itr)->component) );
					if( ret != nullptr ){
						lst.push_back( { ret, *itr });
					}
				}
				return lst;
			}

			template<class TYPE>
			std::list<TYPE *> listAllComponentsOfAType( ) {
				std::list<TYPE *> lst;

				std::list<RegisteredComponent *> & vector = listRegisteredComponents();

				for( auto itr = vector.begin(); itr != vector.end(); itr++ ) {
					TYPE * ret = dynamic_cast<TYPE *>( const_cast<Component*>((*itr)->component) );
					if( ret != nullptr ){
						lst.push_back( ret );
					}
				}
				return lst;
			}


			const uint32_t getMaximumComponentNumber() {
				return maxComponentNumber;
			}

			/*
			 * Free all allocated components and call their shutdown() method.
			 */
			void shutdown();

			// stop processing of threads
			void stop();

			// stop all modules EXCEPT for mandatory modules, these can be stopped calling stop()
			void stopNonMandatoryModules();

			// start processing processing of threads
			void start();

		private:
			uint32_t maxComponentNumber = 0;
			std::map<ComponentReferenceID, RegisteredComponent*> map;
			std::list<RegisteredComponent*> list;
	};

}

#endif