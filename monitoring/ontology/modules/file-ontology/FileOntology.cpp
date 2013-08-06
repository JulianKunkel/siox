#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>

#include <monitoring/ontology/OntologyImplementation.hpp>
#include <core/datatypes/VariableDatatypeSerializable.hpp>


#include <boost/archive/xml_oarchive.hpp> 
#include <boost/archive/xml_iarchive.hpp> 
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>



#include "FileOntologyOptions.hpp"


using namespace std;

using namespace monitoring;
using namespace core;
using namespace boost;


CREATE_SERIALIZEABLE_CLS(FileOntologyOptions)



namespace monitoring{

class AttributeWithValues{
public:
	OntologyAttribute attribute;
	vector<OntologyAttributeID> meta_attributes;
	vector<OntologyValue> values;
};

class FileOntology: public Ontology{

	void load(string filename){
		domain_name_map.clear();
		attribute_map.clear();

		ifstream file(filename);
		if(! file.good())
			return;
		try{
			boost::archive::xml_iarchive archive(file, boost::archive::no_header | boost::archive::no_codecvt);
			archive >> boost::serialization::make_nvp("MAX_VALUE", nextID);
			archive >> boost::serialization::make_nvp("map", attribute_map);

			// recreate domain_name_map
			for(auto itr = attribute_map.begin(); itr != attribute_map.end(); itr++){
				auto pair = *itr;
				AttributeWithValues * av = pair.second;

				stringstream unique(av->attribute.domain);
				unique << "|" << av->attribute.name;
				string fqn(unique.str());

				domain_name_map[fqn] = av; 
			}
		}catch(boost::archive::archive_exception e){
			cerr << "Input file " << filename << " is damaged, recreating ontology!" << endl;
			domain_name_map.clear();
			attribute_map.clear();
			nextID = 1;
		}

		file.close();
	}

	void save(string filename){
		ofstream file(filename);
		boost::archive::xml_oarchive archive(file, boost::archive::no_header | boost::archive::no_codecvt);
		archive << boost::serialization::make_nvp("MAX_VALUE", nextID);
		archive << boost::serialization::make_nvp("map", attribute_map);

		file.close();
	}

	void init(){
		FileOntologyOptions & o = getOptions<FileOntologyOptions>();
		filename = o.filename;
		if (filename.length() == 0){
			filename = "ontology.dat";
		}
		cout << "Initializing file ontology using " << filename << endl;

		load(filename);
	}

	ComponentOptions * AvailableOptions() {
		return new FileOntologyOptions();
	}

	void shutdown(){
		save(filename);

		for(auto itr = attribute_map.begin(); itr != attribute_map.end(); itr++){
			auto pair = *itr;
			AttributeWithValues * av = pair.second;		
			delete(av);
		}
	}

	///////////////////////////////////////////////////


    const OntologyAttribute & register_attribute(const string & domain, const string & name, VariableDatatype::Type storage_type) throw(IllegalStateError){
    	// lookup if the domain + name exists in the table.
		stringstream unique(domain);
		unique << "|" << name;
		string fqn(unique.str());

		if (domain_name_map[fqn] == nullptr){
			attributeMutex.lock();
			if (domain_name_map[fqn] == nullptr){// Necessary due to possible race conditions!
				AttributeWithValues * av = new AttributeWithValues();
				av->attribute.aID = nextID++;
				av->attribute.name = name;
				av->attribute.domain = domain; 
				av->attribute.storage_type = storage_type;
				domain_name_map[fqn] = av;
				attribute_map[av->attribute.aID] = av;
			}
			attributeMutex.unlock();

		}

		// check for consistency:
		if(storage_type != domain_name_map[fqn]->attribute.storage_type ){
			throw IllegalStateError("Storage type does not match");
		}

		return domain_name_map[fqn]->attribute;
    }

    void attribute_set_meta_attribute(const OntologyAttribute & att, const OntologyAttribute & meta, const OntologyValue & value) throw(IllegalStateError){
    	assert( meta.storage_type == value.type() );

    	attributeMutex.lock();
    	// check if the attribute has been set in the past:    	
    	int prevIndex = lookupAttributeIndex(att, meta);

	   	auto stored = attribute_map[att.aID];    	
    	if( prevIndex >= 0){
    		attributeMutex.unlock();
    		if( stored->values[prevIndex] == value )
    			return;
    		else
    			throw IllegalStateError("Value is different!");
    	}
	   	stored->meta_attributes.push_back(meta.aID);
	   	stored->values.push_back(value);
	   	attributeMutex.unlock();
    }

    const OntologyAttribute & lookup_attribute_by_name(const string & domain, const string & name) const throw(NotFoundError){
		stringstream unique(domain);
		unique << "|" << name;

		auto res = domain_name_map.find(unique.str());

		if (res != domain_name_map.end()){
			return res->second->attribute;
		}else{
			throw NotFoundError();
		}
    }

    const OntologyAttribute & lookup_attribute_by_ID(OntologyAttributeID aID) const throw(NotFoundError){

		auto res = attribute_map.find(aID);

		if (res != attribute_map.end()){
			return res->second->attribute;
		}else{
			throw NotFoundError();
		}
    }

    const vector<OntologyAttributeID> & enumerate_meta_attributes(const OntologyAttribute & attribute) const throw(NotFoundError){
    	auto res = attribute_map.find(attribute.aID);

		if (res != attribute_map.end()){
			return res->second->meta_attributes;
		}else{
			throw NotFoundError();
		}
    }

    const OntologyValue & lookup_meta_attribute(const OntologyAttribute & attribute, const OntologyAttribute & meta) const throw(NotFoundError){
    	// check for existing types:
    	int which = lookupAttributeIndex(attribute, meta);
    	if (which >= 0){
				return  attribute_map.find(attribute.aID)->second->values[which];
		}
    	throw NotFoundError("Datatype not found");
    }

private:
	OntologyAttributeID nextID = 1; 

	mutex attributeMutex;

	map<OntologyAttributeID, AttributeWithValues*> attribute_map;
	// map domain + "|" + name to the AttributeWithValues if possible
	map<string, AttributeWithValues*> domain_name_map;

	string filename;

    int lookupAttributeIndex(const OntologyAttribute & attribute, const OntologyAttribute & meta) const {
    	// check for existing types:
    	auto list = attribute_map.find(attribute.aID)->second->meta_attributes;
    	OntologyAttributeID searchFor = meta.aID;

    	int which = 0;
		for(auto it= list.begin(); it != list.end(); it++){
			if( *it == searchFor){
				return which;
			}
			which++;
		}

		return -1;
	}
};

}



namespace boost{
namespace serialization {
template<class Archive>
void serialize(Archive & ar, AttributeWithValues & g, const unsigned int version)
{
	ar & boost::serialization::make_nvp("attribute", g.attribute);
	ar & boost::serialization::make_nvp("metas", g.meta_attributes);
	ar & boost::serialization::make_nvp("values", g.values);
}

template<class Archive>
void serialize(Archive & ar, OntologyAttribute & g, const unsigned int version)
{
	ar & boost::serialization::make_nvp("id", g.aID);
	ar & boost::serialization::make_nvp("name", g.name);
	ar & boost::serialization::make_nvp("domain", g.domain);
	ar & boost::serialization::make_nvp("storageType", g.storage_type);
}

}
}
CREATE_SERIALIZEABLE_CLS_EXTERNAL(AttributeWithValues)
CREATE_SERIALIZEABLE_CLS_EXTERNAL(OntologyAttribute)

COMPONENT(FileOntology)
