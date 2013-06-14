#ifndef CONTAINER_MACROS_HPP
#define CONTAINER_MACROS_HPP

#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/assume_abstract.hpp>

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

#define MEMBER(x) & BOOST_SERIALIZATION_NVP(x)
#define PARENT_CLASS(x) & BOOST_SERIALIZATION_BASE_OBJECT_NVP(x)

#endif
