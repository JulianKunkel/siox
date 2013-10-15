#ifndef CORE_VARIABLE_DATATYPE_JBINARY_SERIALIZABLE_HPP
#define CORE_VARIABLE_DATATYPE_JBINARY_SERIALIZABLE_HPP

#include <core/container/container-binary-serializer.hpp>
#include <core/datatypes/VariableDatatype.hpp>

class VariableDatatypeAccessor {
	public:
		union VariableDatatype::Data * data;
		enum VariableDatatype::Type * typeP;

		VariableDatatypeAccessor( VariableDatatype & e ) {
			typeP = & e.type_;
			data = & e.data;
		}
};

namespace j_serialization{

inline uint64_t serializeLen(const VariableDatatype & obj){
	uint64_t count = 0;
	VariableDatatypeAccessor g( (VariableDatatype &) obj );

	count += serializeLen(* (int8_t*) g.typeP); // ;

	switch( *g.typeP ) {
			case( VariableDatatype::Type::INT64 ):
				count += serializeLen( g.data->i64 );
				break;
			case( VariableDatatype::Type::INT32 ):
				count += serializeLen( g.data->i32 );
				break;
			case( VariableDatatype::Type::UINT64 ):
				count += serializeLen( g.data->ui64 );
				break;
			case( VariableDatatype::Type::UINT32 ):
				count += serializeLen( g.data->ui32 );
				break;
			case( VariableDatatype::Type::STRING ):
				count += serializeLen( g.data->str );
				break;
			case( VariableDatatype::Type::INVALID ):
				break;
			case( VariableDatatype::Type::DOUBLE ):
				count += serializeLen( g.data->d );
				break;
			case( VariableDatatype::Type::FLOAT ):
				count += serializeLen( g.data->f );
				break;
	}
	return count;
}

inline void serialize(const VariableDatatype & obj, char * buffer, uint64_t & pos){

	VariableDatatypeAccessor g( (VariableDatatype &) obj );

	serialize(* (int8_t*) g.typeP, buffer, pos ); // ;

	switch( *g.typeP ) {
			case( VariableDatatype::Type::INT64 ):
				serialize( g.data->i64, buffer, pos);
				break;
			case( VariableDatatype::Type::INT32 ):
				serialize( g.data->i32, buffer, pos);
				break;
			case( VariableDatatype::Type::UINT64 ):
				serialize( g.data->ui64, buffer, pos);
				break;
			case( VariableDatatype::Type::UINT32 ):
				serialize( g.data->ui32, buffer, pos);
				break;
			case( VariableDatatype::Type::STRING ):
				serialize( g.data->str, buffer, pos);
				break;
			case( VariableDatatype::Type::INVALID ):
				break;
			case( VariableDatatype::Type::DOUBLE ):
				serialize( g.data->d, buffer, pos);
				break;
			case( VariableDatatype::Type::FLOAT ):
				serialize( g.data->f, buffer, pos);
				break;
	}
}

inline void deserialize(VariableDatatype & obj, const char * buffer, uint64_t & pos, uint64_t length){
	VariableDatatypeAccessor g( (VariableDatatype &) obj );

	deserialize(* (int8_t*) g.typeP, buffer, pos, length );

	switch( *g.typeP ) {
			case( VariableDatatype::Type::INT64 ):
				deserialize( g.data->i64, buffer, pos, length);
				break;
			case( VariableDatatype::Type::INT32 ):
				deserialize( g.data->i32, buffer, pos, length);
				break;
			case( VariableDatatype::Type::UINT64 ):
				deserialize( g.data->ui64, buffer, pos, length);
				break;
			case( VariableDatatype::Type::UINT32 ):
				deserialize( g.data->ui32, buffer, pos, length);
				break;
			case( VariableDatatype::Type::STRING ):
				deserialize( g.data->str, buffer, pos, length);
				break;
			case( VariableDatatype::Type::INVALID ):
				break;
			case( VariableDatatype::Type::DOUBLE ):
				deserialize( g.data->d, buffer, pos, length);
				break;
			case( VariableDatatype::Type::FLOAT ):
				deserialize( g.data->f, buffer, pos, length);
				break;
	}
}
}

#endif