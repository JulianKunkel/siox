/**
 * @file    ActivityBuilder.hpp
 *
 * @description This class manages everything for building an Activity.
 * @standard    Preferred standard is C++11
 *
 * @created: Jul 8, 2013
 * @author: mickler
 *
 */

#ifndef ACTIVITYBUILDER_HPP_
#define ACTIVITYBUILDER_HPP_

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/datatypes/ids.hpp>

namespace monitoring {

class ActivityBuilder
{
public:
	ActivityBuilder();
	~ActivityBuilder();

	// Local activities
	ActivityID * startActivity(ComponentID *cid);
	void stopActivity(ActivityID *aid);
	void endActivity(ActivityID *aid);
	void addActivityAttribute(ActivityID *aid, Attribute *attribute);
	void reportActivityError(ActivityID *aid, int64_t error);

	// Remote activities
	siox_remote_call * setupRemoteCall(ActivityID *aid, NodeID *target_node_id, UniqueInterfaceID *target_unique_interface_id, AssociateID *target_associate_id);
	void addRemoteCallAttribute(siox_remote_call *remote_call, Attribute *attribute, void *value);
	void startRemoteCall(siox_remote_call *remote_call);
	void startActivityFromRemoteCall(ActivityID *aid, NodeID *caller_node_id, UniqueInterfaceID *caller_unique_interface_id, AssociateID *caller_associate_id);
};

}

#endif /* ACTIVITYBUILDER_HPP_ */
