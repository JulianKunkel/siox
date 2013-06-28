#ifndef SIOX_COMPONENT_S_H
#define SIOX_COMPONENT_S_H

#include <core/component/Component.hpp>
#include <core/container/container-macros.hpp>


#define SERIALIZE_CONTAINER(VAR_) SERIALIZE(VAR_ PARENT_CLASS(Container))

using namespace core;


/**
 * Describes a module to load. 
 * Add this datatype to componentOptions to enable loading of the module by using the AutoConfigurator.
 */
template<class MODULETYPE>
class Module{
public:
	// name of the module
	std::string name;
	// path to search for the module
	std::string path;
	// required interface
	std::string interface;

	MODULETYPE * instance = NULL;

    template <typename Archive>
	void serialize(Archive& ar, const unsigned int version){
		ar & BOOST_SERIALIZATION_NVP(name);
		ar & BOOST_SERIALIZATION_NVP(path);
		ar & BOOST_SERIALIZATION_NVP(interface);

		boost::serialization::split_member(ar, *this, version);
	}

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		// just save the options of the child module
		if (instance != NULL){
			Component * c = (Component*) instance;
			ComponentOptions * o = c->get_options();
			delete(o);
		}
	}

	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		// Instantiate child module and load it..
		// TODO
	}


};



#endif
