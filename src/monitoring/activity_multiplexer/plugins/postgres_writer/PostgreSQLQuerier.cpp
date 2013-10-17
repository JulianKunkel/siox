#include <monitoring/activity_multiplexer/plugins/postgres_writer/PostgreSQLQuerier.hpp>


PostgreSQLQuerier::PostgreSQLQuerier(PGconn &dbconn) : dbconn_(&dbconn)
{
}


/**
 * Returns the stored Activity given its ActivityID.
 */
Activity *PostgreSQLQuerier::query_activity(const ActivityID &aid)
{
	uint64_t uid = query_activity_unique_id(aid);
	return query_activity(uid);
}


/**
 * Returns the stored Activity given its unique_id.
 */
Activity *PostgreSQLQuerier::query_activity(const uint64_t uid)
{
	uint64_t unique_id = util::htonll(uid);
	
	const int nparams = 1;
	const char *param_values[nparams] = {(char *) &unique_id};
	const int param_lengths[nparams] = {sizeof(unique_id)};
	const int param_formats[nparams] = {1};
	const int result_format = 1;
	
	const char *command = 
		"SELECT *                                                      \
		 FROM activity.activities AS a, activity.activity_ids as b  \
		 WHERE                                                         \
			a.unique_id = $1::int8 AND a.unique_id = b.unique_id";

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cerr << "Error loading activity: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
	}

	if (PQntuples(res) == 0) {
		std::cerr << "Error loading activity: unique_id not found." << std::endl;
		PQclear(res);
		return 0;
	}

	Activity *act = load_activity(res, 0);
	
	PQclear(res);
	
	return act;
}


vector<RemoteCall> *PostgreSQLQuerier::query_activity_remote_calls(const uint64_t unique_id)
{
	vector<RemoteCall> *remote_calls = new vector<RemoteCall>();

	uint64_t uid = util::htonll(unique_id);
	
	const int nparams = 1;
	const char *param_values[nparams] = {(char *) &uid};
	const int param_lengths[nparams] = {sizeof(uid)};
	const int param_formats[nparams] = {1};
	const int result_format = 1;
	
	const char *command = 
		"SELECT *                                                       \
		 FROM activity.remote_call_ids AS i, activity.remote_calls AS r \
		 WHERE                                                          \
			i.unique_id = r.unique_id AND i.activity_uid = $1::int8;";

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cerr << "Error loading activity remote calls: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
	}

	for (int i = 0; i < PQntuples(res); i++) {
		remote_calls->push_back(*load_remote_call(res, i));
	}
	
	return remote_calls;
}


RemoteCallIdentifier *PostgreSQLQuerier::load_remote_call_id(PGresult *res, const int i)
{
	if (!res) {
		std::cout << "Empty result set." << std::endl;
		return 0;
	}
	
	if (PQntuples(res) < i) {
		std::cout << "Activity index out of range." << std::endl;
		return 0;
	}

	int nid_pos = PQfnumber(res, "nid");
	int uuid_pos = PQfnumber(res, "uuid");
	int instance_pos = PQfnumber(res, "instance");
	
	char *nid_ptr = PQgetvalue(res, i, nid_pos);
	char *uuid_ptr = PQgetvalue(res, i, uuid_pos);
	char *instance_ptr = PQgetvalue(res, i, instance_pos);
	
	uint32_t nid = ntohl(*((uint32_t *) nid_ptr));
	uint32_t uuid = ntohl(*((uint32_t *) uuid_ptr));
	uint32_t instance = ntohl(*((uint32_t *) instance_ptr));

	RemoteCallIdentifier *rcid = new RemoteCallIdentifier(nid, uuid, instance);
	
	return rcid;
}


RemoteCall *PostgreSQLQuerier::load_remote_call(PGresult *res, const int i)
{
	int attributes_pos = PQfnumber(res, "attributes");
	char *attributes_ptr = PQgetvalue(res, i, attributes_pos);

	RemoteCall *rc = new RemoteCall;
	rc->target = *load_remote_call_id(res, i);
	rc->attributes = *unserialize_attributes(attributes_ptr);
	
	return rc;
}


/**
 * Creates an Activity from a PostgreSQL result structure.
 */
Activity *PostgreSQLQuerier::load_activity(PGresult *res, const int i)
{
	if (!res) {
		std::cout << "Empty result set." << std::endl;
		return 0;
	}
	
	if (PQntuples(res) < i) {
		std::cout << "Activity index out of range." << std::endl;
		return 0;
	}

	int unique_id_pos = PQfnumber(res, "unique_id");
	int ucaid_pos = PQfnumber(res, "ucaid");
	int time_start_pos = PQfnumber(res, "time_start");
	int time_stop_pos = PQfnumber(res, "time_stop");
	int error_value_pos = PQfnumber(res, "error_value");
	int attributes_pos = PQfnumber(res, "attributes");
	
	char *unique_id_ptr = PQgetvalue(res, i, unique_id_pos);
	char *ucaid_ptr = PQgetvalue(res, i, ucaid_pos);
	char *time_start_ptr = PQgetvalue(res, i, time_start_pos);
	char *time_stop_ptr = PQgetvalue(res, i, time_stop_pos);
	char *error_value_ptr = PQgetvalue(res, i, error_value_pos);
	char *attributes_ptr = PQgetvalue(res, i, attributes_pos);
	
	uint64_t unique_id = util::ntohll(*((uint64_t *) unique_id_ptr));
	uint32_t ucaid = ntohl(*((uint32_t *) ucaid_ptr));
	uint64_t time_start = util::ntohll(*((uint64_t *) time_start_ptr));
	uint64_t time_stop  = util::ntohll(*((uint64_t *) time_stop_ptr));
	uint32_t error_value = ntohl(*((uint32_t *) error_value_ptr));
	
	ActivityID *aid = load_activity_id(res, i);
	
	vector<Attribute> *attributes = unserialize_attributes(attributes_ptr);
	vector<RemoteCall> *remote_calls = query_activity_remote_calls(unique_id);
	vector<ActivityID> *parents = query_activity_parents(unique_id);
	RemoteCallIdentifier *invoker = 0;

	Activity *act = new Activity(ucaid, time_start, time_stop, *aid, *parents, *attributes, *remote_calls, invoker, error_value);
	
	return act;
}


/**
 * Utility function to convert a postgres result set in a SIOX activity id.
 */
ActivityID *PostgreSQLQuerier::load_activity_id(PGresult *res, const int i)
{
	if (!res) {
		std::cerr << "Empty result set." << std::endl;
		return 0;
	}
	
	if (PQntuples(res) < i) {
		std::cerr << "ActivityID index out of range." << std::endl;
		return 0;
	}

	int           id_pos = PQfnumber(res, "id");
	int    thread_id_pos = PQfnumber(res, "thread_id");
	int       cid_id_pos = PQfnumber(res, "cid_id");
	int  cid_pid_nid_pos = PQfnumber(res, "cid_pid_nid");
	int  cid_pid_pid_pos = PQfnumber(res, "cid_pid_pid");
	int cid_pid_time_pos = PQfnumber(res, "cid_pid_time");

	char           *id = PQgetvalue(res, i, id_pos);
	char    *thread_id = PQgetvalue(res, i, thread_id_pos);
	char       *cid_id = PQgetvalue(res, i, cid_id_pos);
	char  *cid_pid_nid = PQgetvalue(res, i, cid_pid_nid_pos);
	char  *cid_pid_pid = PQgetvalue(res, i, cid_pid_pid_pos);
	char *cid_pid_time = PQgetvalue(res, i, cid_pid_time_pos);
	
	ActivityID *aid = new ActivityID;
	
	aid->id           = ntohl(*((uint32_t *) id));
	aid->thread       = ntohl(*((uint32_t *) thread_id));
	aid->cid.id       = ntohl(*((uint32_t *) cid_id));
	aid->cid.pid.nid  = ntohl(*((uint32_t *) cid_pid_nid));
	aid->cid.pid.pid  = ntohl(*((uint32_t *) cid_pid_pid));
	aid->cid.pid.time = ntohl(*((uint32_t *) cid_pid_time));
	
	return aid;
}


vector<ActivityID> *PostgreSQLQuerier::query_activity_parents(const uint64_t unique_id)
{
	vector<ActivityID> *parents = new vector<ActivityID>();
	
	uint64_t id = util::htonll(unique_id);

	const int nparams = 1;
	const char *param_values[nparams] = {(char *) &id};
	const int param_lengths[nparams] = {sizeof(id)};
	const int param_formats[nparams] = {1};
	const int result_format = 1;
	const char *command = 
		"SELECT *                                                      \
		 FROM activity.activity_ids AS a, activity.parents AS b     \
		 WHERE a.unique_id = b.child_id AND a.unique_id = $1::int8;";
	
	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);
	
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		
		std::cerr << "Error loading activity parents: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
	}
	
	for (int i = 0; i < PQntuples(res); i++) {
		parents->push_back(*load_activity_id(res, i));
	}
	
	return parents;
}


uint64_t PostgreSQLQuerier::query_activity_unique_id(const ActivityID &aid)
{
	uint32_t id = htonl(aid.id);
	uint32_t thread = htonl(aid.thread);
	uint32_t cid = htonl(aid.cid.id);
	uint32_t nid = htonl(aid.cid.pid.nid);
	uint32_t pid = htonl(aid.cid.pid.pid);
	uint32_t time = htonl(aid.cid.pid.time);
	
	const int nparams = 6;
	const char *param_values[nparams] = {(char *) &id, (char *) &thread, (char *) &cid, (char *) &nid, (char *) &pid, (char *) &time};
	const int param_lengths[nparams] = {sizeof(id), sizeof(thread), sizeof(cid), sizeof(nid), sizeof(pid), sizeof(time)};
	const int param_formats[nparams] = {1, 1, 1, 1, 1, 1};
	const int result_format = 1;

	const char *command = 
		"SELECT unique_id                                              \
		 FROM activity.activity_ids                                    \
		 WHERE                                                         \
			id = $1::int4 AND thread_id = $2::int4                 \
			AND cid_id = $3::int4 AND cid_pid_nid = $4::int4       \
			AND cid_pid_pid = $5::int4 AND cid_pid_time = $6::int4";
		

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		PQclear(res);
		return 0;
	}
	
	if (PQntuples(res) != 1) {
		PQclear(res);
		return 0;
	}

	char *uid_ptr = PQgetvalue(res, 0, 0);
	uint64_t uid = util::ntohll(*((uint64_t *) uid_ptr));
	
	PQclear(res);

	return uid;
}


uint64_t PostgreSQLQuerier::query_remote_call_unique_id(const RemoteCallIdentifier &rcid)
{
	uint32_t nid = htonl(rcid.nid);
	uint32_t uuid = htonl(rcid.uuid);
	uint32_t instance = htonl(rcid.instance);
	
	const int nparams = 3;
	const char *param_values[nparams] = {(char *) &nid, (char *) &uuid, (char *) &instance};
	const int param_lengths[nparams] = {sizeof(nid), sizeof(uuid), sizeof(instance)};
	const int param_formats[nparams] = {1, 1, 1};
	const int result_format = 1;

	const char *command = 
		"SELECT unique_id                                              \
			FROM activity.remote_call_ids AS id                    \
			WHERE                                                  \
				id.nid = $1::int4 AND id.uuid = $2::int4 AND id.instance = $3::int4;";
		
	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		PQclear(res);
		return 0;

	}
	
	if (PQntuples(res) != 1) {
		PQclear(res);
		return 0;
	}
	
	char *uid_ptr = PQgetvalue(res, 0, 0);
	uint64_t uid = util::ntohll(*((uint64_t *) uid_ptr));
	
	PQclear(res);

	return uid;
}


ActivityID *PostgreSQLQuerier::query_activity_id(const uint64_t uid)
{
	uint64_t unique_id = util::htonll(uid);
	
	const int nparams = 1;
	const char *param_values[nparams] = {(char *) &unique_id};
	const int param_lengths[nparams] = {sizeof(unique_id)};
	const int param_formats[nparams] = {1};
	const int result_format = 1;

	const char *command = "SELECT * FROM activity.activity_ids WHERE unique_id = $1::int8;";
		
	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		PQclear(res);
		return 0;
	}

	if (PQntuples(res) != 1) {
		PQclear(res);
		return 0;
	}

	ActivityID *aid = load_activity_id(res, 0);
	
	PQclear(res);

	return aid;

}


uint64_t PostgreSQLQuerier::insert_remote_call_id(const RemoteCallIdentifier &rcid, const uint64_t activity_uid)
{
	uint64_t unique_id = query_remote_call_unique_id(rcid);
	
	if (unique_id != 0)
		return unique_id;

	uint32_t nid = htonl(rcid.nid);
	uint32_t uuid = htonl(rcid.uuid);
	uint32_t instance = htonl(rcid.instance);
	uint64_t auid = util::htonll(activity_uid);
	
	const int nparams = 4;
	const char *param_values[nparams] = {(char *) &nid, (char *) &uuid, (char *) &instance, (char *) &auid};
	const int param_lengths[nparams] = {sizeof(nid), sizeof(uuid), sizeof(instance), sizeof(auid)};
	const int param_formats[nparams] = {1, 1, 1, 1};
	const int result_format = 1;

	const char *command =
	    "INSERT INTO activity.remote_call_ids (                           \
			nid, uuid, instance, activity_uid                      \
		) VALUES (                                                     \
			$1::int4, $2::int4, $3::int4, $4::int8                 \
		) RETURNING unique_id";

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cerr << "Error inserting remote call id: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
	}

	if (PQntuples(res) != 1) {
		std::cerr << "Error inserting remote call id: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
	}

	char *unique_id_ptr = PQgetvalue(res, 0, 0);
	unique_id = util::ntohll(*((uint64_t *) unique_id_ptr));

	PQclear(res);

	return unique_id;

}


uint64_t PostgreSQLQuerier::insert_activity_id(const ActivityID &aid)
{
	uint64_t unique_id = query_activity_unique_id(aid);

	if (unique_id != 0)
		return unique_id;

	uint32_t       id = htonl(aid.id);
	uint32_t   thread = htonl(aid.thread);
	uint32_t   cid_id = htonl(aid.cid.id);
	uint32_t  cid_nid = htonl(aid.cid.pid.nid);
	uint32_t  cid_pid = htonl(aid.cid.pid.pid);
	uint32_t cid_time = htonl(aid.cid.pid.time);

	const int nparams = 6;
	const char *param_values[nparams] = {
		(char *) &id,
		(char *) &thread,
		(char *) &cid_id,
		(char *) &cid_nid,
		(char *) &cid_pid,
		(char *) &cid_time
	};

	const int param_lengths[nparams] = {
		sizeof(id),
		sizeof(thread),
		sizeof(cid_id),
		sizeof(cid_nid),
		sizeof(cid_pid),
		sizeof(cid_time)
	};

	const int param_formats[nparams] = {1,   1,  1,  1,  1,  1};
	const int result_format = 1;

	const char *command =
	    "INSERT INTO activity.activity_ids (                              \
			id, thread_id,                                         \
			cid_id, cid_pid_nid, cid_pid_pid, cid_pid_time         \
		) VALUES (                                                     \
			$1::int4, $2::int4,                                    \
			$3::int4, $4::int4, $5::int4, $6::int4                 \
		) RETURNING unique_id";

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		
		std::cout << "Error inserting activity: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;
		
	}

	char *unique_id_ptr = PQgetvalue(res, 0, 0);
	unique_id = util::ntohll(*((uint64_t *) unique_id_ptr));
	
	PQclear(res);

	return unique_id;

}


uint64_t PostgreSQLQuerier::insert_activity(const Activity &act)
{
	uint64_t uid = insert_activity_id(act.aid());
	
	insert_activity_parents(uid, act.parentArray());
	vector<uint64_t> *rcids = insert_remote_calls(act.remoteCallsArray(), uid);
	
	uint64_t unique_id = util::htonll(uid);
	uint32_t ucaid = htonl(act.ucaid());
	uint64_t time_start = util::htonll(act.time_start());
	uint64_t time_stop = util::htonll(act.time_stop());
	uint32_t error_value = htonl(act.errorValue());

	std::string attributes = serialize_attributes(act.attributeArray());
	std::string remote_calls = serialize_ids(*rcids);
	
	const int nparams = 7;
	const char *param_values[nparams] = {
		(char *) &unique_id,
		(char *) &ucaid,
		(char *) &time_start,
		time_stop   == 0 ? NULL : (char *) &time_stop,
		error_value == 0 ? NULL : (char *) &error_value,
		attributes.length() == 0 ? NULL : attributes.c_str(),
		remote_calls.length() == 0 ? NULL : remote_calls.c_str()
	};

	const int param_lengths[nparams] = {
		sizeof(unique_id),
		sizeof(ucaid),
		sizeof(time_start),
		sizeof(time_stop),
		sizeof(error_value),
		(int) attributes.length(),
		(int) remote_calls.length()
	};

	const int param_formats[nparams] = {1, 1, 1, 1, 1, 0, 0};
	const int result_format = 0;

	const char *command =
	    "INSERT INTO activity.activities (                               \
			unique_id,                                             \
			ucaid, time_start, time_stop, error_value, attributes, remote_calls  \
		) VALUES (                                                     \
			$1::int8,                                              \
			$2::int4, $3::int8, $4::int8, $5::int4, $6, $7         \
		)";

	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {

		std::cerr << "Error inserting activity: " << PQresultErrorMessage(res) << std::endl;
		PQclear(res);
		return 0;

	}

	PQclear(res);

	return uid;

}


void PostgreSQLQuerier::insert_activity_parent(uint64_t child_uid, const ActivityID &aid)
{
	uint64_t parent_uid = insert_activity_id(aid);

	parent_uid = util::htonll(parent_uid);
	child_uid = util::htonll(child_uid);

	const int nparams = 2;
	const char *param_values[nparams] = {(char *) &parent_uid, (char *) &child_uid};
	const int param_lengths[nparams] = {sizeof(parent_uid), sizeof(child_uid)};
	const int param_formats[nparams] = {1, 1};
	const int result_format = 0;

	const char *command =
		"INSERT INTO activity.parents (parent_id, child_id)          \
			VALUES ($1::int8, $2::int8);";


	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		
		std::cerr << "Error inserting parent: " << PQresultErrorMessage(res) << std::endl;
	}

	PQclear(res);

}


void PostgreSQLQuerier::insert_activity_parents(const uint64_t child_id, const vector<ActivityID> &parents)
{
	if (!parents.empty()) {

		std::vector<ActivityID>::const_iterator i;
		
		for (i = parents.begin(); i < parents.end(); ++i) {
			insert_activity_parent(child_id, (*i));
		}

	}

}
 
 
vector<uint64_t> *PostgreSQLQuerier::insert_remote_calls(const vector<RemoteCall> &remote_calls, uint64_t activity_uid)
{
	vector<uint64_t> *rcids = new vector<uint64_t>();

	if (!remote_calls.empty()) {

		vector<RemoteCall>::const_iterator i;
		
		for (i = remote_calls.begin(); i < remote_calls.end(); ++i) 
			rcids->push_back(insert_remote_call((*i), activity_uid));
	}

	return rcids;
}


uint64_t PostgreSQLQuerier::insert_remote_call(const RemoteCall &rc, const uint64_t activity_uid)
{
	uint64_t uid = insert_remote_call_id(rc.target, activity_uid);
	uint64_t unique_id = util::htonll(uid);

	std::string attributes = serialize_attributes(rc.attributes);
	
	const int nparams = 2;
	const char *param_values[nparams] = {
		(char *) &unique_id, 
		attributes.length() == 0 ? NULL : attributes.c_str()
	};
	const int param_lengths[nparams] = {sizeof(unique_id), (int) attributes.length()};
	const int param_formats[nparams] = {1, 0};
	const int result_format = 0;

	const char *command =
		"INSERT INTO activity.remote_calls (unique_id, attributes)   \
			VALUES ($1::int8, $2);";


	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		
		std::cerr << "Error inserting parent: " << PQresultErrorMessage(res) << std::endl;
	}

	PQclear(res);

	return uid;
}

 
std::string PostgreSQLQuerier::serialize_attributes(const vector<Attribute> &attributes)
{
	std::stringstream attr_string;

	if (attributes.empty())
		return attr_string.str();

// 	attr_string << "{";

	std::vector<Attribute>::const_iterator i;

	for (i = attributes.begin(); i < attributes.end(); ++i) {

		attr_string << "\"";
		attr_string << (*i).id << ",";
		attr_string << (*i).value;
		attr_string << "\"";

		if (i != attributes.end() - 1)
			attr_string << ";";

	}

// 	attr_string << "}";

	return attr_string.str();
}


vector<Attribute> *PostgreSQLQuerier::unserialize_attributes(char *attributes_str)
{
	vector<Attribute> *attributes = new vector<Attribute>;
	
	if (strlen(attributes_str) < 3)
		return attributes;

	char *pch = strtok(attributes_str, ",");
	
	while (pch != NULL) {
		
		OntologyAttributeID attr_id = strtoul(pch+1, NULL, 10);
		pch = strtok(NULL, "\""); 
		AttributeValue attr_value = strtoul(pch, NULL, 10);
		pch = strtok(NULL, ",");
		
		if (pch != NULL)
			pch++;

		Attribute attr(attr_id, attr_value);
		attributes->push_back(attr);
	}
	
	return attributes;
}


std::string PostgreSQLQuerier::serialize_ids(const vector<uint64_t> items)
{
	std::stringstream str;

	if (items.empty())
		return str.str();

	str << "{";

	std::vector<uint64_t>::const_iterator i;

	for (i = items.begin(); i < items.end(); ++i) {

		str << *i;

		if (i != items.end() - 1)
			str << ", ";

	}

	str << "}";

	return str.str();
}


