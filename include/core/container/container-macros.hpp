#ifndef CONTAINER_MACROS_HPP
#define CONTAINER_MACROS_HPP

#include <core/container/container.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/factory.hpp>

// http://www.boost.org/doc/libs/1_53_0/libs/serialization/doc/traits.html
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
// To actually create the templates the archive must be created
// see http://www.boost.org/doc/libs/1_53_0/libs/serialization/doc/special.html
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

/*
 Use the following macros to enable automatic serialization/parsing of configuration information.
 */

#define SERIALIZE(VAR_) \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize(Archive & ar, const unsigned int /* file_version */){ \
	ar VAR_ \
    ;\
    }


/*
 * Use this macro if the serialization is not done by the derived class, but relies on the virtual base class "Container"
 * It will instantiate the necessary templates.
 */
#define CREATE_SERIALIZEABLE_CLS(CLS_) \
 	BOOST_SERIALIZATION_FACTORY_0(CLS_)\
 	BOOST_CLASS_EXPORT(CLS_)\
 	BOOST_CLASS_IMPLEMENTATION(CLS_, boost::serialization::object_serializable)\
 	BOOST_CLASS_TRACKING(CLS_, boost::serialization::track_never) \
 	template void CLS_::serialize(boost::archive::xml_oarchive & ar, const unsigned int version); \
	template void CLS_::serialize(boost::archive::xml_iarchive & ar, const unsigned int version);




#define MEMBER(x) & BOOST_SERIALIZATION_NVP(x)
#define PARENT_CLASS(x) & BOOST_SERIALIZATION_BASE_OBJECT_NVP(x)



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

	MODULETYPE * instance;

    template <typename Archive>
	void serialize(Archive& ar, const unsigned int version){
	}
	
};




#endif
