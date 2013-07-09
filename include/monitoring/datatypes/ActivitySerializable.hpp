/**
 * This derived class enables serialization of an Activity using boost.

 * @author Julian Kunkel
 * @date   2013
 */

#ifndef SIOX_ACTIVITY_SER_H
#define SIOX_ACTIVITY_SER_H

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/datatypes/ids-serialize.hpp>

using namespace std;

namespace boost{
namespace serialization {
template<class Archive>
void serialize(Archive & ar,  monitoring::RemoteCall & id, const unsigned int file_version){
	SER("t", id.target)
	SER("a", id.attributeArray)
}

template<class Archive>
void serialize(Archive & ar, monitoring::RemoteCallIdentifier & id, const unsigned int file_version){
	SER("h", id.hwid)
	SER("uuid", id.uuid)
	SER("i", id.instance)
}

template<class Archive>
void serialize(Archive & ar, monitoring::Attribute & id, const unsigned int file_version){
	SER("o", id.id)
	// TODO value depends on the type of the attribute which is available in the ontology cache...
	// Right now use a boost::variant
	SER("v", id.value)
}



}
}

namespace monitoring {

class ActivitySerializable : public Activity{
public:
	/* Not needed we simply cast an Activity to an ActivitySerializable, kind of an hack.
	ActivitySerializable(string name, uint64_t start_t, uint64_t end_t, ComponentID cid, 
		vector<ComponentID> * parentArray, 
		vector<Attribute> * attributeArray, 
		vector<RemoteCall> * remoteCallsArray, 
		RemoteCallIdentifier * remoteInvokee,  int32_t errorValue): Activity(name, start_t, end_t, cid, (parentArray), (attributeArray), (remoteCallsArray), remoteInvokee, errorValue){}
	*/

private:
	// allow serialization of this class
	friend class boost::serialization::access;

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		//RemoteCallIdentifier * remoteInvokee_; // NULL if none
		//ComponentID * parentArray_; // NULL terminated
		//RemoteCall * cur = remoteCallsArray_;

		bool do_serialize = (remoteInvoker_ != nullptr);
		ar << boost::serialization::make_nvp("is_r", do_serialize);
		if( do_serialize ){
			ar << boost::serialization::make_nvp("remote", remoteInvoker_);
		}
	}

	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		bool serialized;
		ar >>  boost::serialization::make_nvp("is_r", serialized);
		if (serialized){
			ar >> boost::serialization::make_nvp("remote", remoteInvoker_);
		}
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version){
		SER("aid", aid_)
		SER("ts", time_start_)
		SER("te", time_stop_)
		SER("err", errorValue_)
		SER("cid", cid_)
		SER("a", 	attributeArray_)
		SER("p", parentArray_)
		SER("r", 	remoteCallsArray_)
		boost::serialization::split_member(ar, *this, file_version);
	}
};

}



#endif // SIOX_ACTIVITY_H
