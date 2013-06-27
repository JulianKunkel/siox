#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>


#include <core/container/container-serializer.hpp>
#include <core/container/container.hpp>
#include <core/container/container-serializable.hpp>

/**
 * ContainerSerializer allows to serialize/deserialize containers which use the macros defined in container-macros.hpp
 * into configuration strings and files.
 */

using namespace std;


namespace core{

	string ContainerSerializer::serialize(const Container * object){
		stringstream s;
		serialize(object, s);
		return s.str();
	}

	void ContainerSerializer::serialize(const Container * object, ostream & os){
	    unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt;
	    assert(os.good());
	    boost::archive::xml_oarchive oa(os, flags);
	    
	    oa << BOOST_SERIALIZATION_NVP(object);
	}

	Container * ContainerSerializer::parse(istream & stream){
		Container * object;
		unsigned int flags = boost::archive::no_header;
		assert(stream.good());
		boost::archive::xml_iarchive ia(stream, flags);
		ia >> BOOST_SERIALIZATION_NVP(object);
		return object;
	}

	Container * ContainerSerializer::parse(string data){
		stringstream s(data);
		return parse(s);
	}

}
