#ifndef CONTAINER_SERIALIZER_HPP
#define CONTAINER_SERIALIZER_HPP

#include <core/container/container.hpp>

#include <sstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/nvp.hpp>


/**
 * ContainerSerializer allows to serialize/deserialize containers which use the macros defined in container-macros.hpp
 * into configuration strings and files.
 */

using namespace std;

namespace core{

template<class CONTAINER>
class ContainerSerializer{
public:
	string serialize(const CONTAINER & object){
		stringstream s;
		serialize(object, s);
		return s.str();
	}

	void serialize(const CONTAINER & object, ostream & os){
	    unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt;
	    assert(os.good());
	    boost::archive::xml_oarchive oa(os, flags);
	    
	    oa << BOOST_SERIALIZATION_NVP(object);
	}

	void parse(CONTAINER & object, istream & stream){
		unsigned int flags = boost::archive::no_header;
		assert(stream.good());
		boost::archive::xml_iarchive ia(stream, flags);
		ia >> BOOST_SERIALIZATION_NVP(object);
	}

	void parse(CONTAINER & object, string data){
		stringstream s(data);
		parse(object, s);
	}
};


}

#endif
