#ifndef CORE_VARIABLE_DATATYPE_SERIALIZABLE_HPP
#define CORE_VARIABLE_DATATYPE_SERIALIZABLE_HPP

#include <boost/serialization/serialization.hpp>

#include <core/datatypes/VariableDatatype.hpp>
#include <core/container/container-macros.hpp>

class VariableDatatypeAssessor 
{
public:
	union VariableDatatype::Data * data;
    enum VariableDatatype::Type * typeP;

    explicit VariableDatatypeAssessor(VariableDatatype &e) { 
    	typeP = & e.type_;
    	data = & e.data;
    }	
};

namespace boost{
namespace serialization {
template<class Archive>
void serialize(Archive & ar, VariableDatatype & a, const unsigned int version)
{	
	VariableDatatypeAssessor g(a);
	ar & boost::serialization::make_nvp("type", *g.typeP);

	switch(*g.typeP){
		case(VariableDatatype::INT64):
		ar & boost::serialization::make_nvp("v", g.data->i64);
		break;
		case(VariableDatatype::INT32):
		ar & boost::serialization::make_nvp("v", g.data->i32);
		break;
		case(VariableDatatype::UINT64):
		ar & boost::serialization::make_nvp("v", g.data->ui64);
		break;
		case(VariableDatatype::UINT32):
		ar & boost::serialization::make_nvp("v", g.data->ui32);
		break;
		case(VariableDatatype::STRING):{
			string s(g.data->str);
			ar & boost::serialization::make_nvp("v", s);
			if( g.data->str == nullptr){
				g.data->str = strdup(s.c_str());
			}
			break;
		}
		case(VariableDatatype::INVALID):
		ar & boost::serialization::make_nvp("v", g.data->ui64);
		break;
		case(VariableDatatype::DOUBLE):
		ar & boost::serialization::make_nvp("v", g.data->d);
		break;
		case(VariableDatatype::FLOAT):
		ar & boost::serialization::make_nvp("v", g.data->f);
		break;
	}
}
}
}

BOOST_CLASS_IMPLEMENTATION(VariableDatatype, boost::serialization::object_serializable) 
BOOST_CLASS_TRACKING(VariableDatatype, boost::serialization::track_never)
BOOST_CLASS_EXPORT(VariableDatatype)

#endif