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
	};


	class ComponentRegistrar {
		public:
			Component * lookupComponent( ComponentReferenceID which ) throw( InvalidComponentException );
			void registerComponent( ComponentReferenceID nr, const std::string & section, const std::string & moduleName, Component * component );

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


			const int numberOfRegisteredComponents() {
				return list.size();
			}

			/*
			 * Free all allocated components and call their shutdown() method.
			 */
			void shutdown();

		private:
			std::map<ComponentReferenceID, RegisteredComponent*> map;
			std::list<RegisteredComponent*> list;
	};

}

#endif