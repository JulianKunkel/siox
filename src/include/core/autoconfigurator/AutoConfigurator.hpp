#ifndef SIOX_AUTOCONFIGURATOR_H
#define SIOX_AUTOCONFIGURATOR_H

#include <core/autoconfigurator/ConfigurationProvider.hpp>
#include <core/component/Component.hpp>
#include <core/component/ComponentRegistrar.hpp>
#include <exception>

#include <vector>
#include <list>
#include <string>

using namespace std;

/*
 * Since this class does provide/parse the configuration it does not need to be derived from Component.
 */


class InvalidConfiguration: public std::exception {
	private:
		string cause;

	public:
		InvalidConfiguration( const string & cause, const string & module ) {
			if( module != "" ) {
				this->cause = "Module \"" + module + "\": " + cause;
			} else {
				this->cause = cause;
			}
		}

		virtual const char * what() const throw() {
			return cause.c_str();
		}
};


namespace core {

	class AutoConfigurator {
		public:
			/*
			 * This constructur is used for fully automatic configuration.
			 * i.e. when the configuration provider will load/know its primary configuration and load new configurations manually.
			 * This is mainly used in application processes
			 *
			 * configuration_entry_point is module specific and allows the module to fetch information from this entry point, e.g. a file.
			 * This is useful, for example, if the user sets a file_configuration upon startup of the daemon.
			 */
			AutoConfigurator( ComponentRegistrar * r, string conf_provider_module, string module_path, string configuration_entry_point );

			/*
			 * This constructur allows to inject a ConfigurationProvider with its own settings.
			 * Mainly for testing purpose.
			 */
			AutoConfigurator( ComponentRegistrar * r, ConfigurationProvider * conf_provider );

			~AutoConfigurator();

			/*
			 * Load/Parse the configuration options, create the necessary components and link their attributes.
			 */
			vector<Component *> LoadConfiguration( string type, string matchingRules ) throw( InvalidComponentException, InvalidConfiguration );

			template<class TYPE>
			TYPE * searchFor( vector<Component *> vector ) {
				for( auto itr = vector.begin(); itr != vector.end(); itr++ ) {
					TYPE * ret = dynamic_cast<TYPE *>( *itr );
					if( ret != nullptr )
						return ret;
				}
				return nullptr;
			}


			template<class TYPE>
			list<TYPE *> searchForAll( vector<Component *> vector ) {
				list<TYPE *> lst;

				for( auto itr = vector.begin(); itr != vector.end(); itr++ ) {
					TYPE * ret = dynamic_cast<TYPE *>( *itr );
					if( ret != nullptr ){
						lst.push_back(ret);
					}
				}
				return lst;
			}

			/*
			 * Create an empty configuration entry for the given component
			 */
			string DumpConfiguration( ComponentOptions * options );

		private:
			ConfigurationProvider * configurationProvider;
			ComponentRegistrar * registrar;
	};


}

#endif
