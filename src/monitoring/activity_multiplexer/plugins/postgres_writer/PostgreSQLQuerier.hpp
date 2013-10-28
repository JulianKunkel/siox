#ifndef POSTGRESQL_DB_QUERIER
#define POSTGRESQL_DB_QUERIER

#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "libpq-fe.h"

#include <util/Util.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <monitoring/transaction_system/DBQuerier.hpp>

/*
 * PostgreSQL OID's:
 *
 * OID      Row-type            Bytes
 * ==================================================
 * 21       smallint            2
 * 20       bigint              8
 * 23       integer             4
 * 1043     character varying(255)
 *
 * Reset all tables:
 *
 * truncate activity, node, hardware, software, instance;
 * alter sequence node_id_seq restart with 1;
 * alter sequence hardware_id_seq restart with 1;
 * alter sequence software_id_seq restart with 1;
 * alter sequence instance_id_seq restart with 1;
 *
 */

class PostgreSQLQuerier : DBQuerier {
public:
	PostgreSQLQuerier(PGconn &dbconn);

	uint64_t insert_activity(const Activity &act);
	Activity *query_activity(uint64_t uid);
	Activity *query_activity(const ActivityID &aid);
	Activity *load_activity(PGresult *res, const int i = 0);

	uint64_t insert_activity_id(const ActivityID &aid);
	ActivityID *query_activity_id(const uint64_t uid);
	uint64_t query_activity_unique_id(const ActivityID &aid);
	ActivityID *load_activity_id(PGresult *res, const int i = 0);
	
	void insert_activity_parents(const uint64_t child_id, const vector<ActivityID> &parents);
	vector<ActivityID> *query_activity_parents(const uint64_t unique_id);
	void insert_activity_parent(uint64_t child_uid, const ActivityID &aid);
	
	vector<uint64_t> *insert_remote_calls(const vector<RemoteCall> &remote_calls, const uint64_t activity_uid);
	vector<RemoteCall> *query_activity_remote_calls(const uint64_t unique_id);
	uint64_t insert_remote_call(const RemoteCall &rc, const uint64_t activity_uid);
	RemoteCall *load_remote_call(PGresult *res, const int i = 0);
	
	uint64_t insert_remote_call_id(const RemoteCallIdentifier &rcid, const uint64_t activity_uid);
	uint64_t query_remote_call_unique_id(const RemoteCallIdentifier &rcid);
	RemoteCallIdentifier *load_remote_call_id(PGresult *res, const int i = 0);
	
	std::string serialize_attributes(const vector<Attribute> &attributes);
	vector<Attribute> *unserialize_attributes(char *attributes_str);
	std::string serialize_ids(const vector<uint64_t> items);

private:
	PGconn *dbconn_;
	
};

#endif
