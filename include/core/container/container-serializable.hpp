#ifndef CONTAINER_SERIALIZABLE_H
#define CONTAINER_SERIALIZABLE_H

#include <boost/serialization/serialization.hpp>

namespace boost{
namespace serialization {
template<class Archive>
void serialize(Archive & ar, core::Container & g, const unsigned int version)
{
}
}
}

BOOST_CLASS_IMPLEMENTATION(core::Container, boost::serialization::object_serializable) 
BOOST_CLASS_TRACKING(core::Container, boost::serialization::track_never)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(core::Container)
BOOST_CLASS_EXPORT(core::Container)

#endif