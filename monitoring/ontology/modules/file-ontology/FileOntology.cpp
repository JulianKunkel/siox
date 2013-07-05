#include <string>
#include <map>
#include <vector>

#include <iostream>
#include <sstream>
#include <mutex>

#include <monitoring/ontology/OntologyImplementation.hpp>
#include "FileOntologyOptions.hpp"


using namespace std;

using namespace monitoring;
using namespace core;


CREATE_SERIALIZEABLE_CLS(FileOntologyOptions)

namespace monitoring{

class AttributeWithValues{
public:
	OntologyAttribute attribute;
	vector<OntologyAttribute*> meta_attributes;
	vector<OntologyValue> values;
};

class FileOntology: public Ontology{

	void load(string filename){

	}

	void save(string filename){

	}

	void init(ComponentOptions * options){
		FileOntologyOptions * o = (FileOntologyOptions*) options;
		filename = o->filename;
		if (filename.length() == 0){
			filename = "ontology.dat";
		}
		cout << "Initializing file ontology using " << filename << endl;

		load(filename);
	}

	ComponentOptions * get_options() {
		return new FileOntologyOptions();
	}

	void shutdown(){
		save(filename);
	}

	///////////////////////////////////////////////////


    OntologyAttribute * register_attribute(string & domain, string & name, enum siox_ont_storage_type storage_type){
    	// lookup if the domain + name exists in the table.
		stringstream unique(domain);
		unique << "|" << name;
		string fqn(unique.str());

		if (domain_name_map[fqn] == NULL){
			attributeMutex.lock();
			if (domain_name_map[fqn] == NULL){
				AttributeWithValues * av = new AttributeWithValues();
				av->attribute.aID = nextID++;
				av->attribute.name = name; 
				av->attribute.storage_type = storage_type;
				domain_name_map[fqn] = av;
				attribute_map[av->attribute.aID] = av;
			}
			attributeMutex.unlock();

		}

		// check for consistency:
		if(storage_type != domain_name_map[fqn]->attribute.storage_type ){
			return nullptr;
		}

		return & domain_name_map[fqn]->attribute;
    }

    bool attribute_set_meta_attribute(OntologyAttribute * att, OntologyAttribute * meta, OntologyValue & value){    	
    	attributeMutex.lock();
    	// check if the attribute has been set in the past:
    	const OntologyValue * val = lookup_meta_attribute(att, meta);
    	if( val != NULL){
    		attributeMutex.unlock();
    		if( *val == value )
    			return true;
    		else
    			return false;
    	}

	   	auto stored = attribute_map[att->aID];
	   	stored->meta_attributes.push_back(meta);
	   	stored->values.push_back(value);
	   	attributeMutex.unlock();

    	return true;
    }

    OntologyAttribute * lookup_attribute_by_name(string & domain, string & name){
		stringstream unique(domain);
		unique << "|" << name;
		return & domain_name_map[unique.str()]->attribute;
    }

    OntologyAttribute * lookup_attribute_by_ID(OntologyAttributeID aID){
		return & attribute_map[aID]->attribute;
    }

    const vector<OntologyAttribute*> & enumerate_meta_attributes(OntologyAttribute * attribute){
    	return attribute_map[attribute->aID]->meta_attributes;
    }

    const OntologyValue * lookup_meta_attribute(OntologyAttribute * attribute, OntologyAttribute * meta){
    	// check for existing types:
    	auto stored = attribute_map[attribute->aID];
    	auto list = stored->meta_attributes;
    	OntologyAttributeID searchFor = meta->aID;

    	int which = 0;
		for(auto it= list.begin(); it != list.end(); it++){
			if( (*it)->aID == searchFor){
				return & stored->values[which];
			}
			which++;
		}
    	return nullptr;
    }


private:
	OntologyAttributeID nextID = 1; 

	mutex attributeMutex;

	map<OntologyAttributeID, AttributeWithValues*> attribute_map;
	// map domain + "|" + name to the AttributeWithValues if possible
	map<string, AttributeWithValues*> domain_name_map;

	string filename;
};

}


COMPONENT(FileOntology)
