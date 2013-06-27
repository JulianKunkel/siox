#ifndef CONTAINER_SERIALIZER_HPP
#define CONTAINER_SERIALIZER_HPP

#include <core/container/container.hpp>

#include <sstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>

/**
 * ContainerSerializer allows to serialize/deserialize containers which use the macros defined in container-macros.hpp
 * into configuration strings and files.
 */

using namespace std;
namespace boost {
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

namespace core{

class ContainerSerializer{
public:
	string serialize(const Container * object){
		stringstream s;
		serialize(object, s);
		return s.str();
	}

	void serialize(const Container * object, ostream & os){
	    unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt;
	    assert(os.good());
	    boost::archive::xml_oarchive oa(os, flags);
	    
	    oa << BOOST_SERIALIZATION_NVP(object);
	}

	Container * parse(istream & stream){
		Container * object;
		unsigned int flags = boost::archive::no_header;
		assert(stream.good());
		boost::archive::xml_iarchive ia(stream, flags);
		ia >> BOOST_SERIALIZATION_NVP(object);
		return object;
	}

	Container * parse(string data){
		stringstream s(data);
		return parse(s);
	}
};


}

#endif
