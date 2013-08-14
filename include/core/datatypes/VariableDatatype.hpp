#ifndef CORE_VARIABLE_DATATYPE
#define CORE_VARIABLE_DATATYPE

#include <string>
#include <string.h>
#include <assert.h>
#include <iostream>


using namespace std;

class VariableDatatypeAccessor;

class VariableDatatype{
public:	
	enum class Type : int8_t {
	    INT64, UINT64, INT32, UINT32, FLOAT, DOUBLE, INVALID, STRING // LONG_DOUBLE not needed right now, it will bloat this structure unnecessarily.
	}; 
protected:

	union Data{
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

	friend VariableDatatypeAccessor;
public:	
	VariableDatatype(){ data.i64 = 0; type_ = Type::INVALID; } 

	VariableDatatype(int64_t i){ data.i64 = i; type_ = Type::INT64;}
	VariableDatatype(uint64_t i){ data.ui64 = i; type_ = Type::UINT64;}
	VariableDatatype(int32_t i){ data.i64 = 0; data.i32 = i; type_ = Type::INT32;}
	VariableDatatype(uint32_t i){ data.i64 = 0; data.ui32 = i; type_ = Type::UINT32;}
	VariableDatatype(const std::string & s){ data.str = strdup(s.c_str()); type_ = Type::STRING; }
	VariableDatatype(const char * str){ data.str = strdup(str); type_ = Type::STRING; }
	VariableDatatype(float f){ data.i64 = 0; data.f = f; type_ = Type::FLOAT;}
	VariableDatatype(double d){ data.d = d; type_ = Type::DOUBLE;}
	//VariableDatatype(long double ld){ data.ld = ld; type_ = LONG_DOUBLE;}

	VariableDatatype(const VariableDatatype & d){ 
		data.i64 = d.data.i64;
		type_ = d.type_;
		if(type_ == Type::STRING){
			data.str = strdup(d.data.str);
		}		
	}

	VariableDatatype(VariableDatatype && d){ // move constructor
		data.i64 = d.data.i64;
		type_ = d.type_;
		if(type_ == Type::STRING){
			data.str = d.data.str;
			d.data.str = nullptr;
			d.type_ = Type::INVALID;
		}
	}

	VariableDatatype & operator=(const VariableDatatype & d){
		//cout << (int) type_ << " " << (int) d.type_ << endl; 
		//At the moment we do not allow to change the datatype during assignments because this is errorprone.
		assert(type_ == d.type_ || type_ == Type::INVALID);
		if(type_ != Type::INVALID){
			if(type_ != Type::STRING){
				data.i64 = d.data.i64;
			}else{
				free(d.data.str);
				data.str = strdup(d.data.str);
			}
		}else{
			type_ = d.type_;
			if(type_ != Type::STRING){
				data.i64 = d.data.i64;
			}else{
				data.str = strdup(d.data.str);
			}
		}

		return *this;
	}


	~VariableDatatype(){
		if (this->type_ == Type::STRING){
			free(this->data.str);
		}
	}

	inline int32_t int32() const {
		assert(type_ == Type::INT32);
		return data.i32;
	}

	inline int64_t int64() const {
		assert(type_ == Type::INT64);
		return data.i64;
	}

	inline uint32_t uint32() const {
		assert(type_ == Type::UINT32);
		return data.ui32;
	}

	inline uint64_t uint64() const {
		assert(type_ == Type::UINT64);
		return data.ui64;
	}

	inline string str() const {
		assert(type_ == Type::STRING);
		return string(data.str);
	}

	inline float flt() const {
		assert(type_ == Type::FLOAT);
		return data.f;
	}

	inline double dbl() const {
		assert(type_ == Type::DOUBLE);
		return data.d;
	}

	//inline long double ldbl() const {
	//	assert(type_ == LONG_DOUBLE);
	//	return data.dd;
	//}

	inline Type type() const {
		return type_;
	}

	inline bool operator==(VariableDatatype const & v) const{
		return v.type_ == this->type_ && ((this->data.i64 == v.data.i64) || (v.type_ == Type::STRING && strcmp(v.data.str, this->data.str) == 0)  ); 
	}

	inline bool operator!=(VariableDatatype const & v) const{
		return ! ( v == *this);
	}
};


inline ostream& operator<<(ostream& os, const VariableDatatype & v)
{
    switch(v.type()){
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
//    	case VariableDatatype::Type::LONG_DOUBLE:
//        	os << v.ldbl();
//            break;
    	case VariableDatatype::Type::INVALID:
        	os << "(INVALID TYPE!)";
            break;
    }
    return os;
}


#endif
