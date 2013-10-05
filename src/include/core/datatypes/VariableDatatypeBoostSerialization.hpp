#ifndef CORE_VARIABLE_DATATYPE_SERIALIZABLE_HPP
#define CORE_VARIABLE_DATATYPE_SERIALIZABLE_HPP

#include <boost/serialization/serialization.hpp>

#include <core/datatypes/VariableDatatype.hpp>
#include <core/container/container-macros.hpp>

class VariableDatatypeAccessor {
	public:
		union VariableDatatype::Data * data;
		enum VariableDatatype::Type * typeP;

		VariableDatatypeAccessor( VariableDatatype & e ) {
			typeP = & e.type_;
			data = & e.data;
		}
};

namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize( Archive & ar, VariableDatatype & a, const unsigned int version )
		{
			VariableDatatypeAccessor g( a );
			SER( "type", *g.typeP );

			switch( *g.typeP ) {
				case( VariableDatatype::Type::INT64 ):
					SER( "v", g.data->i64 );
					break;
				case( VariableDatatype::Type::INT32 ):
					SER( "v", g.data->i32 );
					break;
				case( VariableDatatype::Type::UINT64 ): {
					SER( "v", g.data->ui64 );
					break;
				}
				case( VariableDatatype::Type::UINT32 ):
					SER( "v", g.data->ui32 );
					break;
				case( VariableDatatype::Type::STRING ): {
					if( g.data->str == nullptr ) {
						string s;
						SER( "v", s );
						g.data->str = strdup( s.c_str() );
					} else {
						string s( g.data->str );
						SER( "v", s );
					}
					break;
				}
				case( VariableDatatype::Type::INVALID ):
					break;
				case( VariableDatatype::Type::DOUBLE ):
					SER( "v", g.data->d );
					break;
				case( VariableDatatype::Type::FLOAT ):
					SER( "v", g.data->f );
					break;
			}
		}
	}
}

BOOST_CLASS_IMPLEMENTATION( VariableDatatype, boost::serialization::object_serializable )
BOOST_CLASS_TRACKING( VariableDatatype, boost::serialization::track_never )
BOOST_CLASS_EXPORT( VariableDatatype )

#endif