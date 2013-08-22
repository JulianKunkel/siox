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

// Code needed to serialize the basis-container
BOOST_CLASS_IMPLEMENTATION( core::Container, boost::serialization::object_serializable )
BOOST_CLASS_TRACKING( core::Container, boost::serialization::track_never )
BOOST_SERIALIZATION_ASSUME_ABSTRACT( core::Container )
BOOST_CLASS_EXPORT( core::Container )

namespace core {

	string ContainerSerializer::serialize( const Container * object )
	{
		stringstream s;
		serialize( object, s );
		return s.str();
	}

	void ContainerSerializer::serialize( const Container * object, ostream & os )
	{
		unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt;
		assert( os.good() );
		boost::archive::xml_oarchive oa( os, flags );

		oa << BOOST_SERIALIZATION_NVP( object );
	}

	Container * ContainerSerializer::parse( istream & stream )
	{
		Container * object;
		unsigned int flags = boost::archive::no_header;
		assert( stream.good() );
		assert( ! stream.eof() );
		boost::archive::xml_iarchive ia( stream, flags );
		try {
			ia >> BOOST_SERIALIZATION_NVP( object );
		} catch( boost::archive::archive_exception & e ) {
			int pos = stream.tellg();

			cerr << e.what() << endl;
			cerr << "Error at: " << pos << endl;
			// output whole stream input
			stream.seekg( 0 );
			char str[pos + 1];
			stream.read( str, pos );
			str[pos] = 0;

			cerr << str << endl;
			cerr << " <<ERROR IS HERE>> " << endl;

			string s;
			while( ! stream.eof() ) {
				stream >> s;
				cerr << s << endl;
			}
			throw e;
		}

		return object;
	}

	Container * ContainerSerializer::parse( string data )
	{
		stringstream s( data );
		Container * obj;
		try {
			obj = parse( s );
		} catch( boost::archive::archive_exception & e ) {
			cerr << "Error while parsing string " << endl;
			cerr << data << endl;
			throw e;
		}
		return obj;
	}

}
