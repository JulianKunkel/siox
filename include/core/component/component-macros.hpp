#ifndef SIOX_COMPONENT_S_H
#define SIOX_COMPONENT_S_H

#include <core/component/Component.hpp>
#include <core/container/container-macros.hpp>
#include <boost/serialization/split_free.hpp>

#include <core/component/ComponentRegistrar.hpp>


#define SERIALIZE_CONTAINER(VAR_) SERIALIZE(VAR_ PARENT_CLASS(Container))

extern core::ComponentRegistrar * autoConfiguratorRegistrar;


namespace boost{
namespace serialization {
	template<class Archive>
	void serialize(Archive & ar, core::ComponentReference & g, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("componentID", g.componentID);
		split_free(ar, g, version);
	}

	template<class Archive>
	void save(Archive & ar, const core::ComponentReference & g, const unsigned int version)
	{}

	template<class Archive>
	void load(Archive & ar,  core::ComponentReference & g, const unsigned int version)
	{
		// load component from ModuleRegistrar
		if( g.componentID != 0){
		 	g.componentID = (core::ComponentReferenceID) autoConfiguratorRegistrar->lookup_component(g.componentID);
		}
	}

}
}
//CREATE_SERIALIZEABLE_CLS_EXTERNAL(core::ComponentReference)

#endif
