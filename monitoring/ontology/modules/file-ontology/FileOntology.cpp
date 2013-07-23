#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>

#include <monitoring/ontology/OntologyImplementation.hpp>

#include <boost/archive/xml_oarchive.hpp> 
#include <boost/archive/xml_iarchive.hpp> 
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>



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
	vector<OntologyAttribute*> meta_attributes;
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

	void init(ComponentOptions * options){
		FileOntologyOptions * o = (FileOntologyOptions*) options;
		filename = o->filename;
		if (filename.length() == 0){
			filename = "ontology.dat";
		}
		cout << "Initializing file ontology using " << filename << endl;

		load(filename);

		delete(options);
	}

	ComponentOptions * get_options() {
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


    OntologyAttribute * register_attribute(const string & domain, const string & name, enum siox_ont_storage_type storage_type){
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
			return nullptr;
		}

		return & domain_name_map[fqn]->attribute;
    }

    bool attribute_set_meta_attribute(OntologyAttribute * att, OntologyAttribute * meta, const OntologyValue & value){    	
    	attributeMutex.lock();
    	// check if the attribute has been set in the past:
    	const OntologyValue * val = lookup_meta_attribute(att, meta);
    	if( val != nullptr){
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

    OntologyAttribute * lookup_attribute_by_name(const string & domain, const string & name){
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
