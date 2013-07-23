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

#include <map>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/datatypes/ids.hpp>

using namespace std;

namespace monitoring {

class ActivityBuilder
{
public:
	ActivityBuilder();
	~ActivityBuilder();

	// Local activities
	ActivityID * startActivity(ComponentID *cid, UniqueComponentActivityID *caid, siox_timestamp *t);
	void stopActivity(ActivityID *aid, siox_timestamp *t);
	void endActivity(ActivityID *aid);
	void addActivityAttribute(ActivityID *aid, Attribute *attribute);
	void reportActivityError(ActivityID *aid, siox_activity_error error);
	void linkActivities(ActivityID *child, ActivityID *parent);

	// Remote activities
	RemoteCallID * setupRemoteCall(ActivityID *aid, NodeID *target_node_id, UniqueInterfaceID *target_unique_interface_id, AssociateID *target_associate_id);
	void addRemoteCallAttribute(RemoteCallID *remote_call, Attribute *attribute);
	void startRemoteCall(RemoteCallID *remote_call, siox_timestamp *t);
	void startActivityFromRemoteCall(ActivityID *aid, NodeID *caller_node_id, UniqueInterfaceID *caller_unique_interface_id, AssociateID *caller_associate_id, siox_timestamp *t);

protected:
	map<ActivityID *, ActivityID *> activities_in_flight;
};

}

#endif /* ACTIVITYBUILDER_HPP_ */
