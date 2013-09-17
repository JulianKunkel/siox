#include "PostgreSQLReader.hpp"

PostgreSQLReader::PostgreSQLReader(const std::string &dbinfo) 
{
	dbconn_ = PQconnectdb(dbinfo.c_str());

	if (PQstatus(dbconn_) != CONNECTION_OK) {

		logger->log(Logger::ERR, "Error connecting to DB: %s.", PQerrorMessage(dbconn_));

		PQfinish(dbconn_);
		
		exit(1);

	}

#ifndef NDEBUG
	logger->log(Logger::NOTICE, "Successfully connected to the database.");
#endif

}


PostgreSQLReader::~PostgreSQLReader() 
{
	PQfinish(dbconn_);
}


ActivityID *PostgreSQLReader::activity_id(uint64_t uid)
{
	uint64_t unique_id = htonll(uid);
	
	const int nparams = 1;
	const char *param_values[nparams] = {(char *) &unique_id};
	const int param_lengths[nparams] = {sizeof(unique_id)};
	const Oid param_types[nparams] =   {20};
	const int param_formats[nparams] = {1};
	const int result_format = 1;

	const char *command = "SELECT * FROM activities.activity_id WHERE unique_id = $1::int8;";
		
	PGresult *res = PQexecParams(dbconn_, command, nparams, NULL, param_values, param_lengths, param_formats, result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		logger->log(Logger::ERR, "DB read failed: %s.", PQresultErrorMessage(res));

		PQclear(res);

		return 0;

	}

	int           id_pos = PQfnumber(res, "id");
	int    thread_id_pos = PQfnumber(res, "thread_id");
	int       cid_id_pos = PQfnumber(res, "cid_id");
	int  cid_pid_nid_pos = PQfnumber(res, "cid_pid_nid");
	int  cid_pid_pid_pos = PQfnumber(res, "cid_pid_pid");
	int cid_pid_time_pos = PQfnumber(res, "cid_pid_time");
	
	if (PQntuples(res) > 1) {
		
		logger->log(Logger::ERR, "More than one activity id returned. Aborting.");
		PQclear(res);

		return 0;
	}

	char           *id = PQgetvalue(res, 0, id_pos);
	char    *thread_id = PQgetvalue(res, 0, thread_id_pos);
	char       *cid_id = PQgetvalue(res, 0, cid_id_pos);
	char  *cid_pid_nid = PQgetvalue(res, 0, cid_pid_nid_pos);
	char  *cid_pid_pid = PQgetvalue(res, 0, cid_pid_pid_pos);
	char *cid_pid_time = PQgetvalue(res, 0, cid_pid_time_pos);
	
	ActivityID *aid = new ActivityID;
	
	aid->id           = ntohl(*((uint32_t *) id));
	aid->thread       = ntohl(*((uint32_t *) thread_id));
	aid->cid.id       = ntohl(*((uint32_t *) cid_id));
	aid->cid.pid.nid  = ntohl(*((uint32_t *) cid_pid_nid));
	aid->cid.pid.pid  = ntohl(*((uint32_t *) cid_pid_pid));
	aid->cid.pid.time = ntohl(*((uint32_t *) cid_pid_time));
	
	PQclear(res);

	return aid;

}
