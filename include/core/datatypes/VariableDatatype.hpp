#ifndef CORE_VARIABLE_DATATYPE
#define CORE_VARIABLE_DATATYPE

#include <string>
#include <string.h>


using namespace std;

class VariableDatatypeAssessor;

class VariableDatatype{
public:	
	enum Type{
	    INT64, UINT64, INT32, UINT32, FLOAT, DOUBLE, STRING
	}; 
protected:

	union Data{
		int32_t i32;
		int64_t i64;
		uint32_t ui32;
		uint64_t ui64;		
		float f;
		double d;
		char * str;
	};

	union Data data;
	enum Type type_;

	friend VariableDatatypeAssessor;
public:	
	VariableDatatype(){ data.i64 = 0; } 	// for serialization

	VariableDatatype(int64_t i){ data.i64 = i; type_ = INT64;}
	VariableDatatype(uint64_t i){ data.ui64 = i; type_ = UINT64;}
	VariableDatatype(int32_t i){ data.i64 = 0; data.i32 = i; type_ = INT32;}
	VariableDatatype(uint32_t i){ data.i64 = 0; data.ui32 = i; type_ = UINT32;}
	VariableDatatype(const std::string & s){ data.str = strdup(s.c_str()); type_ = STRING; }
	VariableDatatype(float f){ data.i64 = 0; data.f = f; type_ = FLOAT;}
	VariableDatatype(double d){ data.d = d; type_ = DOUBLE;}

	~VariableDatatype(){
		if (this->type_ == STRING){
			free(this->data.str);
		}
	}

	inline int32_t int32() const {
		assert(type_ == INT32);
		return data.i32;
	}

	inline int64_t int64() const {
		assert(type_ == INT64);
		return data.i64;
	}

	inline uint32_t uint32() const {
		assert(type_ == UINT32);
		return data.ui32;
	}

	inline uint64_t uint64() const {
		assert(type_ == UINT64);
		return data.ui64;
	}

	inline string str() const {
		assert(type_ == STRING);
		return string(data.str);
	}

	inline float flt() const {
		assert(type_ == FLOAT);
		return data.f;
	}

	inline double dbl() const {
		assert(type_ == DOUBLE);
		return data.d;
	}

	inline Type type() const {
		return type_;
	}

	inline bool operator==(VariableDatatype const & v) const{
		return v.type_ == this->type_ && ((this->data.i64 == v.data.i64) || (v.type_ == STRING && strcmp(v.data.str, this->data.str) == 0)  ); 
	}

	inline bool operator!=(VariableDatatype const & v) const{
		return ! ( v == *this);
	}
};


#endif