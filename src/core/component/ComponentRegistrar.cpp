#include <assert.h>

#include <iostream>

#include <core/component/ComponentRegistrar.hpp>

using namespace std;

namespace core {
	Component * ComponentRegistrar::lookupComponent( ComponentReferenceID which ) throw( InvalidComponentException )
	{
		RegisteredComponent * c = map[which];
		if( c == nullptr ) {
			throw InvalidComponentException( which );
		}
		return const_cast<Component*>(c->component);
	}

	void ComponentRegistrar::registerComponent( ComponentReferenceID nr, const string & section, const string & moduleName, Component * component )
	{
		assert( nr != 0 ); // 0 is an invalid / none component
		assert( map[nr] == nullptr );
		RegisteredComponent * rc = new RegisteredComponent( nr, section, moduleName, component );
		list.push_back( rc );
		map[nr] = rc;

		maxComponentNumber = (maxComponentNumber >= nr) ? maxComponentNumber : nr;
	}


	void ComponentRegistrar::unregisterComponent( ComponentReferenceID nr )
	{
		assert( nr != 0 ); // 0 is an invalid / none component
		assert( map[nr] != nullptr );
		for( auto itr = list.begin(); itr != list.end(); itr++ ){
			if( (*itr)->id == nr ){
				list.erase( itr );
				map.erase(map.find(nr));
				delete( *itr );
				break;
			}
		}
	}	

	void ComponentRegistrar::shutdown()
	{
		for( auto it = list.rbegin(); it != list.rend(); it++ ) {
			//cout << "Shutting down " << (*it)->moduleName << endl;
			const_cast<Component*>((*it)->component)->finalize();
			delete( (*it)->component );
			delete( *it );
		}
	}

	void ComponentRegistrar::stop()
	{
		for( auto it = list.rbegin(); it != list.rend(); it++ ) {
			Component* c = const_cast<Component*>((*it)->component);
			//cout << "Shutting down " << (*it)->moduleName << endl;
			if (c->isStarted()){
				c->stop();
			}
		}
	}

	void ComponentRegistrar::stopNonMandatoryModules()
	{
		for( auto it = list.rbegin(); it != list.rend(); it++ ) {
			Component* c = const_cast<Component*>((*it)->component);
			//cout << "Shutting down " << (*it)->moduleName << endl;
			if (c->isStarted() && ! c->isMandatoryModule() ){
				c->stop();
			}
		}
	}

	void ComponentRegistrar::start()
	{
		for( auto it = list.rbegin(); it != list.rend(); it++ ) {
			//cout << "Shutting down " << (*it)->moduleName << endl;
			Component* c = const_cast<Component*>((*it)->component);
			if (! c->isStarted()){
				c->start();
			}
		}
	}
};
