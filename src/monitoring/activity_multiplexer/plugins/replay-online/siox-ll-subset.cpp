#include <C/siox-ll.h>			// implementation provided by siox-ll-subset.cpp

//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
	static VariableDatatype convert_attribute( OntologyAttribute & oa, const void * value )
	{
		AttributeValue v;

		switch( oa.storage_type ) {
			case( VariableDatatype::Type::UINT32 ):
				return *( ( uint32_t * ) value );
			case( VariableDatatype::Type::INT32 ): {
				return *( ( int32_t * ) value );
			}
			case( VariableDatatype::Type::UINT64 ):
				return *( ( uint64_t * ) value );
			case( VariableDatatype::Type::INT64 ): {
				return *( ( int64_t * ) value );
			}
			case( VariableDatatype::Type::FLOAT ): {
				return  *( ( float * ) value );
			}
			case( VariableDatatype::Type::DOUBLE ): {
				return  *( ( double * ) value );
			}
			case( VariableDatatype::Type::STRING ): {
				return ( char * ) value;
			}
			case( VariableDatatype::Type::INVALID ): {
				assert( 0 );
			}
		}
		return "";
	}

static bool convert_attribute_back( OntologyAttribute & oa, const VariableDatatype & val, void * out_value ){
	switch( val.type() ) {
		case VariableDatatype::Type::INT32:
			*((int32_t*) out_value) = val.int32();
			return true;
		case VariableDatatype::Type::UINT32:
			*((uint32_t*) out_value) = val.uint32();
			return true;
		case VariableDatatype::Type::INT64:
			*((int64_t*) out_value) = val.int64();
			return true;
		case VariableDatatype::Type::UINT64:
			*((uint64_t*) out_value) = val.uint64();
			return true;
		case VariableDatatype::Type::FLOAT:
			*((float*) out_value) = val.flt();
			return true;
		case VariableDatatype::Type::DOUBLE:
			*((double*) out_value) = val.dbl();
			return true;
		case VariableDatatype::Type::STRING: {
			*(char**) out_value = strdup(val.str());
			return true;
		}
		case VariableDatatype::Type::INVALID:
		default:
			assert(0 && "tried to optimize for a VariableDatatype of invalid type");
			return false;
	}
}	
