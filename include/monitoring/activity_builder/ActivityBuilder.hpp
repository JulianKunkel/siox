/**
 * @file    ActivityBuilder.hpp
 *
 * This class manages everything for building an Activity.
 *
 * @date: 2013-07-08
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
private:
	vector<Activity*> activity_stack;
	uint32_t next_activity_id;

protected:
	ActivityBuilder();

public:
	static ActivityBuilder* getNewInstance();
	static ActivityBuilder* getThreadInstance();
	~ActivityBuilder();

	// Local activities
	Activity* startActivity(const ComponentID* cid, const UniqueComponentActivityID* ucaid, const Timestamp* t);
	Activity* startActivity(const ComponentID* cid, const UniqueComponentActivityID* ucaid, const NodeID* caller_node_id, const UniqueInterfaceID* caller_unique_interface_id, const AssociateID* caller_associate_id, const Timestamp* t);
	void stopActivity(Activity* a, const Timestamp* t);
	void endActivity(Activity* a);
	void setActivityAttribute(Activity* a, const Attribute* attribute);
	void reportActivityError(Activity* a, const ActivityError error);
	void linkActivities(Activity* child, ActivityID* parent);

	// Remote activities
	RemoteCall* setupRemoteCall(Activity* a, const NodeID* target_node_id, const UniqueInterfaceID* target_unique_interface_id, const AssociateID* target_associate_id);
	void setRemoteCallAttribute(RemoteCall* remote_call, const Attribute* attribute);
	void startRemoteCall(RemoteCall* &remote_call, const Timestamp* t);

protected:
	map<uint32_t, Activity *> activities_in_flight;
};

}

#endif /* ACTIVITYBUILDER_HPP_ */
