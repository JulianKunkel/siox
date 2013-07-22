#include <string.h>

#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>

#include <monitoring/association_mapper/AssociationMapperImplementation.hpp>

#include <boost/archive/xml_oarchive.hpp> 
#include <boost/archive/xml_iarchive.hpp> 
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

#include "FileAssociationMapperOptions.hpp"

using namespace core;
using namespace std;

CREATE_SERIALIZEABLE_CLS(FileAssociationMapperOptions)


template<int LENGTH>
struct ByteRangeComparator
{
    bool operator() (void * const a, void * const b)
    {
        return memcmp(a,b, LENGTH) < 0;  
    }
};


namespace monitoring{


class AttributeIDsAndValues{
public:
	vector<OntologyAttributeID> ids;
	vector<OntologyValue> values;
};


class FileAssociationMapper: public AssociationMapper{
public:
	void save(string filename){
		ofstream file(filename);
		boost::archive::xml_oarchive archive(file, boost::archive::no_header | boost::archive::no_codecvt);
		archive << boost::serialization::make_nvp("LastAssociateID", lastID);
		archive << boost::serialization::make_nvp("AssociateMap", map_str_aid);
		archive << boost::serialization::make_nvp("ProcessMap", map_processAttributes);
		archive << boost::serialization::make_nvp("ComponentMap", map_componentAttributes);
	
		file.close();
	}

	void load(string filename){
		ifstream file(filename);
		if( ! file.good())
			return;
		boost::archive::xml_iarchive archive(file, boost::archive::no_header | boost::archive::no_codecvt);
		archive >> boost::serialization::make_nvp("LastAssociateID", lastID);
		archive >> boost::serialization::make_nvp("AssociateMap", map_str_aid);
		archive >> boost::serialization::make_nvp("ProcessMap", map_processAttributes);
		archive >> boost::serialization::make_nvp("ComponentMap", map_componentAttributes);

		file.close();

		map_aid_str.resize(map_str_aid.size());

		// recreate  map_aid_str
		for(auto itr = map_str_aid.begin(); itr != map_str_aid.end(); itr++){			
			cout << itr->second << " " << itr->first << endl;
			map_aid_str[itr->second - 1] = itr->first;
		}
	}

	void init(ComponentOptions * options){
		FileAssociationMapperOptions * o = (FileAssociationMapperOptions*) options;
		filename = o->filename;
		load(filename);
	}

	ComponentOptions * get_options() {
		return new FileAssociationMapperOptions();
	}

	void shutdown(){
		save(filename);
	}

	OntologyValue * lookup_process_attribute(ProcessID * pid, OntologyAttribute * att){
		AttributeIDsAndValues * av = map_processAttributes[pid];	
		return lookup_attribute(av, att);
	}

	bool set_process_attribute(ProcessID * pid, OntologyAttribute * att, const  OntologyValue & value){
		globalMutex.lock();
		AttributeIDsAndValues * av = map_processAttributes[pid];
		if( av == nullptr){
			av = new AttributeIDsAndValues();
			map_processAttributes[pid] = av;
		}
		bool ret = set_attribute(av, att, value);
		globalMutex.unlock();
		return ret;
	}


	bool set_component_attribute(ComponentID * cid, OntologyAttribute * att,const  OntologyValue & value){
		globalMutex.lock();
		AttributeIDsAndValues * av = map_componentAttributes[cid];	
		if( av == nullptr){
			av = new AttributeIDsAndValues();
			map_componentAttributes[cid] = av;
		}

		bool ret = set_attribute(av, att, value);

		globalMutex.unlock();

		return ret ;
	}

	OntologyValue * lookup_component_attribute(ComponentID * cid, OntologyAttribute * att){
		AttributeIDsAndValues * av = map_componentAttributes[cid];	
		return lookup_attribute(av, att);
	}


	/* These functions are used to create the RemoteInstanceID */
	AssociateID create_instance_mapping(const string & value){

		globalMutex.lock();
		AssociateID last = map_str_aid[value];
		if( last != 0 ){ // !!!! see also lookup_instance_mapping
			globalMutex.unlock();
			return last;
		}	
		last=lastID++; // TODO make thread-safe
		map_str_aid[value] = last;
		map_aid_str.push_back(value);		
		globalMutex.unlock();

		return last;
	}

	const string * lookup_instance_mapping(AssociateID id){
		return & map_aid_str[id - 1]; // !!!!!
	}


private:
	bool set_attribute(AttributeIDsAndValues * av, OntologyAttribute * att, const  OntologyValue & value){
		OntologyAttributeID searchFor = att->aID;

    	int which = 0;
		for(auto it= av->ids.begin(); it != av->ids.end(); it++){
			if( *it == searchFor){
				// check if the value is identical
				if (value == av->values[which]){
					return true;
				}
				return false;
			}
			which++;
		}
		// not found we append it
		av->ids.push_back(searchFor);
	   	av->values.push_back(value);

		return true;
	}

	OntologyValue * lookup_attribute(AttributeIDsAndValues * av, OntologyAttribute * att){
		if(av == nullptr)
			return nullptr;

    	auto list = av->ids;
    	OntologyAttributeID searchFor = att->aID;

    	int which = 0;
		for(auto it= list.begin(); it != list.end(); it++){
			if( *it == searchFor){
				return & av->values[which];
			}
			which++;
		}
		return nullptr;
	}

	mutex globalMutex;

	string filename;

	AssociateID lastID = 1;
	
	map<ProcessID*, AttributeIDsAndValues*, ByteRangeComparator<sizeof(ProcessID)> > map_processAttributes;
	map<ComponentID*, AttributeIDsAndValues*, ByteRangeComparator<sizeof(ComponentID)> > map_componentAttributes;

	map<string, AssociateID> map_str_aid;
	vector<string> map_aid_str;
};

}



namespace boost{
namespace serialization {
template<class Archive>
void serialize(Archive & ar, AttributeIDsAndValues & g, const unsigned int version)
{
	ar & boost::serialization::make_nvp("attids", g.ids);
	ar & boost::serialization::make_nvp("values", g.values);
}


template<class Archive>
void serialize(Archive & ar, ProcessID & g, const unsigned int version)
{
	ar & boost::serialization::make_nvp("nid", g.nid);
	ar & boost::serialization::make_nvp("pid", g.pid);
	ar & boost::serialization::make_nvp("time", g.time);
}


template<class Archive>
void serialize(Archive & ar, ComponentID & g, const unsigned int version)
{
	ar & boost::serialization::make_nvp("pid", g.pid);
	ar & boost::serialization::make_nvp("num", g.num);
}



}
}
CREATE_SERIALIZEABLE_CLS_EXTERNAL(AttributeIDsAndValues)
CREATE_SERIALIZEABLE_CLS_EXTERNAL(ProcessID)
CREATE_SERIALIZEABLE_CLS_EXTERNAL(ComponentID)



COMPONENT(FileAssociationMapper)
