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
	static ActivityBuilder* getInstance();
	~ActivityBuilder();

	// Local activities
	Activity* startActivity(ComponentID* cid, UniqueComponentActivityID* ucaid, siox_timestamp* t);
	void stopActivity(Activity* a, siox_timestamp* t);
	void endActivity(Activity* a);
	void addActivityAttribute(Activity* a, Attribute* attribute);
	void reportActivityError(Activity* a, siox_activity_error error);
	void linkActivities(Activity* child, ActivityID* parent);

	// Remote activities
	RemoteCall* setupRemoteCall(Activity* a, NodeID* target_node_id, UniqueInterfaceID* target_unique_interface_id, AssociateID* target_associate_id);
	void addRemoteCallAttribute(RemoteCall* remote_call, Attribute* attribute);
	void startRemoteCall(RemoteCall* remote_call, siox_timestamp* t);
	Activity* startActivityFromRemoteCall(ComponentID* cid, UniqueComponentActivityID* ucaid, NodeID* caller_node_id, UniqueInterfaceID* caller_unique_interface_id, AssociateID* caller_associate_id, siox_timestamp* t);

protected:
	map<uint32_t, Activity *> activities_in_flight;
};

}

#endif /* ACTIVITYBUILDER_HPP_ */
