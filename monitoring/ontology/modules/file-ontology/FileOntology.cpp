#include <string>
#include <map>
#include <vector>

#include <iostream>

#include <monitoring/ontology/Ontology.hpp>
#include "FileOntologyOptions.hpp"


using namespace std;

using namespace monitoring;
using namespace core;


namespace monitoring{

class AttributePlusValue : siox_attribute_t{
public:
	Value value;

	AttributePlusValue(string & name, enum siox_ont_storage_type storage_type, Value & value) : siox_attribute_t(name, storage_type), value(value) {}
};

class VectorMetric : public siox_metric_t{
public:
	vector<AttributePlusValue> attributes;

	VectorMetric(string & name, string & unit, enum siox_ont_storage_type storage_type) : siox_metric_t(name, unit, storage_type){}
};

class FileOntology: public Ontology{

	void init(ComponentOptions * options){
		FileOntologyOptions * o = (FileOntologyOptions*) options;
		string filename = o->filename;
		if (filename.length() == 0){
			filename = "ontology.dat";
		}
		cout << "Initializing file ontology using " << filename << endl;

		// TODO deserialize data from file
	}

	ComponentOptions * get_options() {
		return new FileOntologyOptions();
	}

	void shutdown(){
		// TODO serialize data into a file
	}

	///////////////////////////////////////////////////

 	siox_metric * register_metric(string & canonical_name, string & unit, enum siox_ont_storage_type storage){
 		assert(metrics_map.find(canonical_name) == metrics_map.end());
 		
 		auto metrics = new VectorMetric(canonical_name, unit, storage);

 		metrics_map[canonical_name] = metrics;
 		return metrics;
 	}

 	siox_metric * find_metric_by_name(string & canonical_name){
 		return metrics_map[canonical_name];
 	}

 	void metric_set_attribute(siox_metric * metric, siox_attribute * attribute, Value & value){
 		VectorMetric * m = (VectorMetric *) metric;
 		cout << "Setting \"" << attribute->name << "\" to: " << value << endl;
 		
 		m->attributes.push_back(AttributePlusValue(attribute->name, attribute->storage_type, value));
  	}

private:
	map<string, VectorMetric*> metrics_map;
};

}


COMPONENT(FileOntology)