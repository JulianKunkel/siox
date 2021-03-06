#ifndef SIOX_COMPONENT_SERIALIZABLE_S_H
#define SIOX_COMPONENT_SERIALIZABLE_S_H

#define SERIALIZE_CONTAINER(VAR_) SERIALIZE(VAR_ PARENT_CLASS(Container))

#ifndef NO_OBJECT_INJECTION

#include <core/component/ComponentReference.hpp>
#include <core/component/ComponentRegistrar.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>

extern core::ComponentRegistrar * autoConfiguratorRegistrar;
extern int autoConfiguratorOffset;

namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize( Archive & ar, ::core::ComponentReference & g, const unsigned int version )
		{
			ar & boost::serialization::make_nvp( "componentID", g.componentID );
			ar & boost::serialization::make_nvp( "global", g.global );
			split_free( ar, g, version );
		}

		template<class Archive>
		void save( Archive & ar, const ::core::ComponentReference & g, const unsigned int version )
		{}

		template<class Archive>
		void load( Archive & ar,  ::core::ComponentReference & g, const unsigned int version )
		{
			// load component from ModuleRegistrar
			if( g.componentID != 0 ) {
				int myOffset = 0;
				if( ! g.global ) {
					myOffset = autoConfiguratorOffset;
				}
				g.componentPointer = ( ::core::Component* ) autoConfiguratorRegistrar->lookupComponent( g.componentID + myOffset );
			}
		}

	}
}

#endif

#endif
