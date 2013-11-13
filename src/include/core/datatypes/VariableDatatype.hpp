#ifndef CORE_VARIABLE_DATATYPE
#define CORE_VARIABLE_DATATYPE

#include <string>
#include <string.h>
#include <assert.h>
#include <iostream>


using namespace std;

class VariableDatatypeAccessor;

//@externalserialization
class VariableDatatype {
	public:
	enum class Type : int8_t {
		    INT64, UINT64, INT32, UINT32, FLOAT, DOUBLE, INVALID, STRING // LONG_DOUBLE not needed right now, it will bloat this structure unnecessarily.
		};
	protected:
		friend VariableDatatypeAccessor;

		union Data {
			int32_t i32;
			int64_t i64;
			uint32_t ui32;
			uint64_t ui64;
			float f;
			double d;
			//long double ld;
			char * str;
		};

		union Data data;
		enum Type type_;

	public:
		VariableDatatype() {
			data.i64 = 0;
			type_ = Type::INVALID;
		}

		VariableDatatype( int64_t i ) {
			data.i64 = i;
			type_ = Type::INT64;
		}
		VariableDatatype( uint64_t i ) {
			data.ui64 = i;
			type_ = Type::UINT64;
		}
		VariableDatatype( int32_t i ) {
			data.i64 = 0;
			data.i32 = i;
			type_ = Type::INT32;
		}
		VariableDatatype( uint32_t i ) {
			data.i64 = 0;
			data.ui32 = i;
			type_ = Type::UINT32;
		}
		VariableDatatype( const std::string & s ) {
			data.str = strdup( s.c_str() );
			type_ = Type::STRING;
		}
		VariableDatatype( const char * str ) {
			data.str = strdup( str );
			type_ = Type::STRING;
		}
		VariableDatatype( float f ) {
			data.i64 = 0;
			data.f = f;
			type_ = Type::FLOAT;
		}
		VariableDatatype( double d ) {
			data.d = d;
			type_ = Type::DOUBLE;
		}
		//VariableDatatype(long double ld){ data.ld = ld; type_ = LONG_DOUBLE;}

		VariableDatatype( const VariableDatatype & d ) {
			data.i64 = d.data.i64;
			type_ = d.type_;
			if( type_ == Type::STRING ) {
				data.str = strdup( d.data.str );
			}
		}

		VariableDatatype( VariableDatatype && d ) { // move constructor
			data.i64 = d.data.i64;
			type_ = d.type_;
			if( type_ == Type::STRING ) {
				data.str = d.data.str;
				d.data.str = nullptr;
				d.type_ = Type::INVALID;
			}
		}

		VariableDatatype & operator=( const VariableDatatype & d ) {
			//cout << (int) type_ << " " << (int) d.type_ << endl;
			//At the moment we do not allow to change the datatype during assignments because this is errorprone.
			assert( type_ == d.type_ || type_ == Type::INVALID );
			if( type_ != Type::INVALID ) {
				if( type_ != Type::STRING ) {
					data.i64 = d.data.i64;
				} else {
					free( d.data.str );
					data.str = strdup( d.data.str );
				}
			} else {
				type_ = d.type_;
				if( type_ != Type::STRING ) {
					data.i64 = d.data.i64;
				} else {
					data.str = strdup( d.data.str );
				}
			}

			return *this;
		}


		~VariableDatatype() {
			if( this->type_ == Type::STRING ) {
				free( this->data.str );
			}
		}

		inline int32_t int32() const {
			assert( type_ == Type::INT32 );
			return data.i32;
		}

		inline int64_t int64() const {
			assert( type_ == Type::INT64 );
			return data.i64;
		}

		inline uint32_t uint32() const {
			assert( type_ == Type::UINT32 );
			return data.ui32;
		}

		inline uint64_t uint64() const {
			assert( type_ == Type::UINT64 );
			return data.ui64;
		}

		inline char * str() const {
			unsigned long long iValue = 0;
			double fValue = 0;
			bool isSigned = false, isFloat = false;
			switch( type_ ) {
				case Type::INT32:  iValue = data.i32; isSigned = true; break;
				case Type::INT64:  iValue = data.i64; isSigned = true; break;
				case Type::UINT32: iValue = data.ui32; break;
				case Type::UINT64: iValue = data.ui32; break;
				case Type::FLOAT:  fValue = data.f; isFloat = true; break;
				case Type::DOUBLE: fValue = data.d; isFloat = true; break;
				case Type::STRING: return data.str;
				case Type::INVALID:
				default:
					assert(0 && "tried to stringify a VariableDatatypes of invalid type"), abort();
			}
			static char resultString[100];
			if(isSigned) {
				snprintf(resultString, sizeof(resultString) - 1, "%lld", (long long)iValue);
			} else if(isFloat) {
				snprintf(resultString, sizeof(resultString) - 1, "%g", fValue);
			} else {
				snprintf(resultString, sizeof(resultString) - 1, "%llu", iValue);
			}
			return resultString;
		}

		inline float flt() const {
			assert( type_ == Type::FLOAT );
			return data.f;
		}

		inline double dbl() const {
			assert( type_ == Type::DOUBLE );
			return data.d;
		}

		//inline long double ldbl() const {
		//  assert(type_ == LONG_DOUBLE);
		//  return data.dd;
		//}

		inline Type type() const {
			return type_;
		}

		inline VariableDatatype& operator+=( VariableDatatype const & v ) {
			if( v.type_ != type_ ) assert(0 && "tried to add VariableDatatypes of unequal type"), abort();

			switch( type_ ) {
				case Type::INT32:
					data.i32 += v.data.i32;
					break;
				case Type::UINT32:
					data.ui32 += v.data.ui32;
					break;
				case Type::INT64:
					data.i64 += v.data.i64;
					break;
				case Type::UINT64:
					data.ui64 += v.data.ui64;
					break;
				case Type::FLOAT:
					data.f += v.data.f;
					break;
				case Type::DOUBLE:
					data.d += v.data.d;
					break;
				case Type::STRING: {
					char* result;
					if(asprintf(&result, "%s%s", data.str, v.data.str) < 0) {
						std::cerr << "Fatal error: asprintf() failed\n";
						abort();
					}
					free(data.str);
					data.str = result;
					break;
				}
				case Type::INVALID:
				default:
					assert(0 && "tried to add VariableDatatypes of invalid type"), abort();
			}
			return *this;
		}

		inline VariableDatatype& operator/=( int64_t divisor ) {
			switch( type_ ) {
				case Type::INT32:
					data.i32 /= divisor;
					break;
				case Type::UINT32:
					data.ui32 /= divisor;
					break;
				case Type::INT64:
					data.i64 /= divisor;
					break;
				case Type::UINT64:
					data.ui64 /= divisor;
					break;
				case Type::FLOAT:
					data.f /= divisor;
					break;
				case Type::DOUBLE:
					data.d /= divisor;
					break;
				case Type::STRING:
					assert(0 && "tried to divide a VariableDatatype of string type"), abort();
				case Type::INVALID:
				default:
					assert(0 && "tried to divide a VariableDatatype of invalid type"), abort();
			}
			return *this;
		}

		inline bool operator<( VariableDatatype const & v ) const {
			if( v.type_ != type_ ) assert(0 && "tried to sort VariableDatatypes of unequal type"), abort();

			switch( type_ ) {
				case Type::INT32:
					return this->data.i32 < v.data.i32;
				case Type::UINT32:
					return this->data.ui32 < v.data.ui32;
				case Type::INT64:
					return this->data.i64 < v.data.i64;
				case Type::UINT64:
					return this->data.ui64 < v.data.ui64;
				case Type::FLOAT:
					return this->data.f < v.data.f;
				case Type::DOUBLE:
					return this->data.d < v.data.d;
				case Type::STRING:
					return strcmp( this->data.str, v.data.str ) < 0;
				case Type::INVALID:
				default:
					assert(0 && "tried to sort VariableDatatypes of invalid type"), abort();
			}
		}

		inline bool operator==( VariableDatatype const & v ) const {

			if( v.type_ != type_ )
				return false;

			switch( type_ ) {
				case Type::INT32:
					return this->data.i32 == v.data.i32;
				case Type::UINT32:
					return this->data.ui32 == v.data.ui32;
				case Type::INT64:
					return this->data.i64 == v.data.i64;
				case Type::UINT64:
					return this->data.ui64 == v.data.ui64;
				case Type::FLOAT:
					return this->data.f == v.data.f;
				case Type::DOUBLE:
					return this->data.d == v.data.d;
				case Type::STRING:
					return strcmp( v.data.str, this->data.str ) == 0;
				case Type::INVALID:
				default:
					return false;
			}

		}

		inline bool operator!=( VariableDatatype const & v ) const {
			return !( v == *this );
		}

		inline void setMax() {
			switch( type_ ) {
				case Type::INT32:
					data.i32 = 0x7fffffff;
					break;
				case Type::UINT32:
					data.ui32 = (uint32_t)-1;
					break;
				case Type::INT64:
					data.i64 = 0x7fffffffffffffffll;
					break;
				case Type::UINT64:
					data.ui64 = (uint64_t)-1;
					break;
				case Type::FLOAT:
					data.f = 1.0/0.0;
					break;
				case Type::DOUBLE:
					data.d = 1.0/0.0;
					break;
				case Type::STRING:
					assert(0 && "tried to set a string to a maximal value, which is not possible"), abort();
					break;
				case Type::INVALID:
				default:
					assert(0 && "tried to set the value of a VariableDatatype of invalid type"), abort();
			}
		}

		inline void setMin() {
			switch( type_ ) {
				case Type::INT32:
					data.i32 = 0x80000000;
					break;
				case Type::UINT32:
					data.ui32 = 0;
					break;
				case Type::INT64:
					data.i64 = 0x8000000000000000ll;
					break;
				case Type::UINT64:
					data.ui64 = 0;
					break;
				case Type::FLOAT:
					data.f = -1.0/0.0;
					break;
				case Type::DOUBLE:
					data.d = -1.0/0.0;
					break;
				case Type::STRING:
					if(data.str) free(data.str);
					data.str = strdup( "" );
					break;
				case Type::INVALID:
				default:
					assert(0 && "tried to set the value of a VariableDatatype of invalid type"), abort();
			}
		}

		inline void setZero() {
			switch( type_ ) {
				case Type::INT32:
				case Type::UINT32:
				case Type::INT64:
				case Type::UINT64:
				case Type::FLOAT:
				case Type::DOUBLE:
					data.ui64 = 0;	//Ain't it nice that the representation of zero is independent of type like this?
					break;
				case Type::STRING:
					if(data.str) free(data.str);
					data.str = strdup( "" );
					break;
				case Type::INVALID:
				default:
					assert(0 && "tried to set the value of a VariableDatatype of invalid type"), abort();
			}
		}
};


inline ostream & operator<<( ostream & os, const VariableDatatype & v )
{
	switch( v.type() ) {
		case VariableDatatype::Type::INT32:
			os << v.int32();
			break;
		case VariableDatatype::Type::INT64:
			os << v.int64();
			break;
		case VariableDatatype::Type::UINT32:
			os << v.uint32();
			break;
		case VariableDatatype::Type::UINT64:
			os << v.uint64();
			break;
		case VariableDatatype::Type::STRING:
			os << v.str();
			break;
		case VariableDatatype::Type::FLOAT:
			os << v.flt();
			break;
		case VariableDatatype::Type::DOUBLE:
			os << v.dbl();
			break;
//      case VariableDatatype::Type::LONG_DOUBLE:
//          os << v.ldbl();
//            break;
		case VariableDatatype::Type::INVALID:
			os << "(INVALID TYPE!)";
			break;
	}
	return os;
}


#endif
