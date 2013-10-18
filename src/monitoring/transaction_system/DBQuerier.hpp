#ifndef DB_QUERIER
#define DB_QUERIER

#include <monitoring/datatypes/Activity.hpp>

using namespace monitoring;

typedef struct {
	NodeID unique_id;
	std::string name;
} SysinfoNode;

typedef struct {
	UniqueInterfaceID unique_id;
	std::string name;
	std::string implementation;
} SysinfoInterface;

typedef struct {
	DeviceID unique_id;
	NodeID nid;
	std::string name;
} SysinfoDevice;


class DBQuerier {
public:
	/** Activities **/
	
	virtual uint64_t insert_activity(const Activity &act) = 0;
	virtual Activity *query_activity(uint64_t uid) = 0;
	virtual Activity *query_activity(const ActivityID &aid) = 0;

	virtual uint64_t insert_activity_id(const ActivityID &aid) = 0;
	virtual ActivityID *query_activity_id(const uint64_t uid) = 0;
	virtual uint64_t query_activity_unique_id(const ActivityID &aid) = 0;
	
	virtual void insert_activity_parents(const uint64_t child_id, const vector<ActivityID> &parents) = 0;
	virtual vector<ActivityID> *query_activity_parents(const uint64_t unique_id) = 0;
	virtual void insert_activity_parent(uint64_t child_uid, const ActivityID &aid) = 0;
	
	virtual vector<uint64_t> *insert_remote_calls(const vector<RemoteCall> &remote_calls, const uint64_t activity_uid) = 0;
	virtual vector<RemoteCall> *query_activity_remote_calls(const uint64_t unique_id) = 0;
	virtual uint64_t insert_remote_call(const RemoteCall &rc, const uint64_t activity_uid) = 0;
	
	virtual uint64_t insert_remote_call_id(const RemoteCallIdentifier &rcid, const uint64_t activity_uid) = 0;
	virtual uint64_t query_remote_call_unique_id(const RemoteCallIdentifier &rcid) = 0;
	
};

#endif
