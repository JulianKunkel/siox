#ifndef SIOX_ONT_H
#define SIOX_ONT_H

#include <string>
#include <memory>

#include <boost/variant.hpp>
//#include <boost/shared_ptr.hpp>

#include <core/component/Component.hpp>

#include <monitoring/datatypes/basic-types.hpp>

using namespace std;

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> Value;

class siox_attribute_t {
public:
    string name;
    enum siox_ont_storage_type storage_type;

    siox_attribute_t(string & name, enum siox_ont_storage_type storage_type) : name(name), storage_type(storage_type) {}
};

class siox_metric_t{
public:
	string canonical_name;
	enum siox_ont_storage_type storage_type;
	string unit;

	siox_metric_t(string & name, string & unit, enum siox_ont_storage_type storage_type) : canonical_name(name), storage_type(storage_type), unit(unit){}
};



namespace monitoring{

class Ontology : public core::Component {

public: 
	virtual siox_attribute * attribute_register(string & name, enum siox_ont_storage_type storage_type){
		return new siox_attribute(name, storage_type); 
	}

	virtual siox_metric * register_metric(string & canonical_name, string & unit, enum siox_ont_storage_type  storage) = 0;

	virtual siox_metric * find_metric_by_name( string & canonical_name) = 0;

	virtual void metric_set_attribute(siox_metric * metric, siox_attribute * attribute, Value & value) = 0;
};


}





// Module interfaces 

#define COMPONENT(x) \
extern "C"{\
void * get_instance_monitoring_ontology() { return new x(); }\
}

#define ONTOLOGY_INTERFACE "monitoring_ontology"



#endif
