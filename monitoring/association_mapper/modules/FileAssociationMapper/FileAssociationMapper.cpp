#include <string.h>

#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>

#include <monitoring/association_mapper/AssociationMapperImplementation.hpp>
#include <core/datatypes/VariableDatatypeSerializable.hpp>


#include <boost/archive/xml_oarchive.hpp> 
#include <boost/archive/xml_iarchive.hpp> 
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

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


template <class TYPE>
struct ByteComparator {
    bool operator()(const TYPE & a, const TYPE & b) const {
        return memcmp(&a, &b, sizeof(TYPE));
    }
};


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
/*! This xml_archive needs for the XML tags these string parameters in " " othewise it would be good enough to use ' archive & lastID; ' */
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

	void init(){
		FileAssociationMapperOptions & o = getOptions<FileAssociationMapperOptions>();
		filename = o.filename;
		assert(filename != "");
		cout << "file " << filename << endl;
		load(filename);
	}

	ComponentOptions * AvailableOptions() {
		return new FileAssociationMapperOptions();
	}

	~FileAssociationMapper(){
		save(filename);
	}

	
	virtual const OntologyValue & lookup_process_attribute(const ProcessID & pid, const OntologyAttribute & att) const throw(NotFoundError){
		auto res = map_processAttributes.find(pid);
		
		if( res != map_processAttributes.end()){
			const OntologyValue * val = lookup_attribute(res->second, att);
			if( val != nullptr){
				return *val;
			}else{
				throw NotFoundError();
			}
		}		
		throw NotFoundError();		
	}


	virtual void set_process_attribute(const ProcessID & pid, const OntologyAttribute & att, const OntologyValue & value) throw(IllegalStateError){
		globalMutex.lock();		
		auto res = map_processAttributes.find(pid);
		AttributeIDsAndValues * av;
		
		if( res == map_processAttributes.end()){
			map_processAttributes[pid] = AttributeIDsAndValues();
			av = & map_processAttributes[pid];
		}
		int ret = set_attribute(av, & att, value);
		globalMutex.unlock();

		if( ret == false){
			throw IllegalStateError("Value exists already!");
		}		
	}


	virtual void set_component_attribute(const ComponentID & cid, const OntologyAttribute & att, const  OntologyValue & value) throw(IllegalStateError){
		globalMutex.lock();
		auto res = map_componentAttributes.find(cid);
		AttributeIDsAndValues * av;
		
		if( res == map_componentAttributes.end()){
			map_componentAttributes[cid] = AttributeIDsAndValues();
			av = & map_componentAttributes[cid];
		}

		bool ret = set_attribute(av, & att, value);

		globalMutex.unlock();

		if( ret == false){
			throw IllegalStateError("Value exists already!");
		}
	}

	virtual const OntologyValue & lookup_component_attribute(const ComponentID & cid, const OntologyAttribute & att) const throw(NotFoundError){
		auto res = map_componentAttributes.find(cid);
		
		if( res != map_componentAttributes.end()){
			const OntologyValue * val = lookup_attribute(res->second, att);
			if( val != nullptr){
				return *val;
			}else{
				throw NotFoundError();
			}
		}		
		throw NotFoundError();
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

	virtual const string & lookup_instance_mapping(AssociateID id) const throw(NotFoundError){
		return map_aid_str[id - 1]; // !!!!!
	}

private:
	bool set_attribute(AttributeIDsAndValues * av, const OntologyAttribute * att, const  OntologyValue & value){
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

	const OntologyValue * lookup_attribute(const AttributeIDsAndValues & av, const OntologyAttribute & att) const{
    	auto list = av.ids;
    	OntologyAttributeID searchFor = att.aID;

    	int which = 0;
		for(auto it= list.begin(); it != list.end(); it++){
			if( *it == searchFor){
				return & av.values[which];
			}
			which++;
		}
		return nullptr;
	}

	mutex globalMutex;

	string filename;

	AssociateID lastID = 1;
	
	map<ProcessID, AttributeIDsAndValues, ByteComparator<ProcessID> > map_processAttributes;
	map<ComponentID, AttributeIDsAndValues, ByteComparator<ComponentID> > map_componentAttributes;

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
	ar & boost::serialization::make_nvp("id", g.id);
}



}
}
CREATE_SERIALIZEABLE_CLS_EXTERNAL(AttributeIDsAndValues)
CREATE_SERIALIZEABLE_CLS_EXTERNAL(ProcessID)
CREATE_SERIALIZEABLE_CLS_EXTERNAL(ComponentID)



COMPONENT(FileAssociationMapper)
