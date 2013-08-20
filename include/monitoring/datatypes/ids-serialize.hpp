/** @file
 * This file contains structures and constructors for SIOX ID, e.g.
 * hardware, processes, components and attributes.
 * @author JK
 */

#ifndef SIOX_IDS_SERIALIZE_H
#define SIOX_IDS_SERIALIZE_H

#include <monitoring/datatypes/ids.hpp>

using namespace monitoring;

#define SER(x,y) ar & boost::serialization::make_nvp(x, y);
namespace boost{
namespace serialization {
	template<class Archive>
	void serialize(Archive & ar, ComponentID & id, const unsigned int file_version){
		cout << "   SERIALIZE " << & id << endl;
		SER("pid", id.pid)
		cout << "FIN" << endl;
		SER("id", id.id)
	}


	template<class Archive>
	void serialize(Archive & ar, ProcessID & id, const unsigned int file_version){
		SER("nid", id.nid)
		SER("pid", id.pid)
		SER("t", id.time);
	}

		
	template<class Archive>
	void serialize(Archive & ar, ActivityID & id, const unsigned int file_version){
		SER("cid", id.cid)
		SER("id", id.id)
		SER("tid", id.thread)
	}
}
}

BOOST_CLASS_TRACKING(ComponentID, boost::serialization::track_never)
BOOST_CLASS_TRACKING(ProcessID, boost::serialization::track_never)
BOOST_CLASS_TRACKING(ActivityID, boost::serialization::track_never)


#endif
