#ifndef CONTAINER_MACROS_HPP
#define CONTAINER_MACROS_HPP

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
#include <boost/serialization/split_member.hpp>


#include <core/container/container.hpp>
#include <core/container/container-serializable.hpp>

#ifdef TEXT_SERIALIZATION
#define SER(x,y) ar & y;
#else
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#define SER(x,y) ar & boost::serialization::make_nvp(x, y);
#endif

/*
 Use the following macros to enable automatic serialization/parsing of configuration information.
 */

#define SERIALIZE(VAR_) \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize(Archive & ar, const unsigned int /* file_version */){ \
	VAR_ \
    }

/*
 * Use this macro if the serialization is not done by the derived class, but relies on the virtual base class "Container"
 * It will instantiate the necessary templates.
 */
#define CREATE_SERIALIZEABLE_CLS(CLS_) \
 	BOOST_CLASS_EXPORT(CLS_)\
 	BOOST_CLASS_IMPLEMENTATION(CLS_, boost::serialization::object_serializable)\
 	BOOST_CLASS_TRACKING(CLS_, boost::serialization::track_never) \
 	template void CLS_::serialize(boost::archive::xml_oarchive & ar, const unsigned int version); \
	template void CLS_::serialize(boost::archive::xml_iarchive & ar, const unsigned int version);

#define CREATE_SERIALIZEABLE_CLS_EXTERNAL(CLS_) \
 	BOOST_CLASS_EXPORT(CLS_)\
 	BOOST_CLASS_IMPLEMENTATION(CLS_, boost::serialization::object_serializable)\
 	BOOST_CLASS_TRACKING(CLS_, boost::serialization::track_never) \
 	template void boost::serialization::serialize(boost::archive::xml_oarchive & ar, CLS_ & g, const unsigned int version); \
	template void boost::serialization::serialize(boost::archive::xml_iarchive & ar, CLS_ & g, const unsigned int version);


#define MEMBER(x) ar & BOOST_SERIALIZATION_NVP(x);

#ifndef NO_OBJECT_INJECTION 
	#define MEMBER_INJECTION(x)
#else
	#define MEMBER_INJECTION(x)  ar & BOOST_SERIALIZATION_NVP(x); 
#endif 

#define PARENT_CLASS(x) ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(x);




#endif
