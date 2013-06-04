#include "PostgreSQLPumpCallback.h"

#include <arpa/inet.h>

PostgreSQLPumpCallback::PostgreSQLPumpCallback(PGconn &dbconn)
   : dbconn_(&dbconn)
{
}


void PostgreSQLPumpCallback::handle_message(ConnectionMessage &msg) const
{
	if (msg.get_msg()->action() == siox::MessageBuffer::Activity) {
		insert_activity(msg);
	}
}


void PostgreSQLPumpCallback::handle_message(boost::shared_ptr<ConnectionMessage> msg) const
{
}


void PostgreSQLPumpCallback::store_node(ConnectionMessage &msg)
{
#ifndef NDEBUG
	logger->log(Logger::NOTICE, "Prepared to insert node.");
#endif
	uint64_t hwid = insert_hwdesc(msg.get_msg()->hwdesc());
	uint64_t swid = insert_swdesc(msg.get_msg()->swdesc());
	uint64_t  iid = insert_indesc(msg.get_msg()->indesc());

	insert_node(hwid, swid, iid);

#ifndef NDEBUG
// 	logger->log(LOG_NOTICE, "Inserted: %" PRIx64 " %" PRIx64 "  %" PRIx64 
// 		    ".", hwid, swid, iid);
#endif
}


void PostgreSQLPumpCallback::store_activity(ConnectionMessage &msg)
{

	uint64_t  aid = htonull(msg.get_msg()->aid());
	//      uint64_t paid = htonull(msg.get_msg()->paid());
	uint64_t unid = htonull(msg.get_msg()->unid());
	uint64_t    tb = htonull(msg.get_msg()->time_start());
	uint64_t    ts = htonull(msg.get_msg()->time_stop());

#ifndef NDEBUG
// 	logger->log(Logger::DEBUG, "Going to insert activity(%" PRIu64 " %" 
// 		    PRIu64 ", %" PRIu64 ", %s)", aid, unid, tb, 
// 		    msg.get_msg()->comment());
#endif
	int nparams = 5;
	const Oid param_types[5] = {20, 20, 20, 20, 1043};
	const char *param_values[5] = {(char *) &aid, (char *) &unid,
					(char *) &tb, (char *) &ts,
					msg.get_msg()->comment().c_str()};
	const int param_lengths[5] = {sizeof(aid), sizeof(unid), sizeof(tb),
				      sizeof(ts), 
				      (int) msg.get_msg()->comment().length()};
	const int param_formats[5] = {1, 1, 1, 1, 0};
	const int result_format = 0;
	const char *command =
		"INSERT INTO activities (aid, unid, time_start, time_stop, comment) \
			VALUES ($1::int8, $2::int8, $3::int8, $4::int8, $5)";

	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));

		PQclear(res);

	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Successfully inserted activity.");
#endif
	PQclear(res);

}

void PostgreSQLPumpCallback::insert_activity(ConnectionMessage &msg) const
{
	uint64_t  aid = htonull(msg.get_msg()->aid());
	uint64_t paid = htonull(msg.get_msg()->paid());
	uint64_t unid = htonull(msg.get_msg()->unid());
	uint64_t   tb = htonull(msg.get_msg()->time_start());
	uint64_t   ts = htonull(msg.get_msg()->time_stop());

#ifndef NDEBUG	
// 	logger->log(Logger::DEBUG, "Going to insert activity(%" PRIu64 " %" 
// 		    PRIu64 ", %" PRIu64 ", %s)", aid, unid, tb, msg.get_msg()->comment().c_str());
#endif
	int nparams = 5;
	const Oid param_types[5] = {20, 20, 20, 20, 1043};
	const char *param_values[5] = {(char *) &aid, (char *) &unid,
				       (char *) &tb, (char *) &ts,
					msg.get_msg()->comment().c_str()};
	const int param_lengths[5] = {sizeof(aid), sizeof(unid), sizeof(tb),
				      sizeof(ts), (int) msg.get_msg()->comment().length()};
	const int param_formats[5] = {1, 1, 1, 1, 0};
	const int result_format = 0;
	const char *command =
		"INSERT INTO activities (aid, unid, time_start, time_stop, comment) \
			VALUES ($1::int8, $2::int8, $3::int8, $4::int8, $5)";

	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));

		PQclear(res);

	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Successfully inserted activity.");
#endif
	PQclear(res);
}


uint64_t PostgreSQLPumpCallback::insert_node(uint64_t hwid, uint64_t swid, 
					     uint64_t iid)
{
#ifndef NDEBUG
// 	logger->log(Logger::DEBUG, "Going to insert node (%" PRIu64 ", %" 
// 		    PRIu64 ", %" PRIu64 ").", hwid, swid, iid);
#endif

	hwid = htonull(hwid);
	swid = htonull(swid);
	iid  = htonull(iid);

	int nparams = 3;
	const Oid param_types[3] = {20, 20, 20};
	const char *param_values[3] = {(char *) &hwid, (char *) &swid,
				       (char *) &iid};
	const int param_lengths[3] = {sizeof(hwid), sizeof(swid), sizeof(iid)};
	const int param_formats[3] = {1, 1, 1};
	const int result_format = 0;
	const char *command =
		"INSERT INTO nodes (hwid, swid, iid) \
			VALUES ($2::int8, $3::int8, $4::int8) \
			RETURNING unid;";
			
	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));
		
		PQclear(res);
		
		return -1;
		
	}
		
	uint64_t retval = strtoll(PQgetvalue(res, 0, 0), NULL, 0);
		
#ifndef NDEBUG
	logger->log(Logger::NOTICE, "Successfully inserted node.");
#endif
	PQclear(res);
		
	return retval;
		
}


uint64_t PostgreSQLPumpCallback::insert_hwdesc(const std::string &hwdesc)
{
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Going to insert hwdesc: %s.", 
		    hwdesc.c_str());
#endif

	int nparams = 1;
	long long int retval;
	const Oid param_types[] = {1043};
	const char *param_values[] = {hwdesc.c_str()};
	const int param_lengths[] = {(int) hwdesc.length()};
	const int *param_formats = {0};
	const int result_format = 0;
	const char *command =
		"INSERT INTO hardware (description) \
			VALUES ($1) \
			RETURNING hwid;";

	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));

		PQclear(res);

		return -1;

	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Successfully inserted node.");
#endif
	
	retval = strtoll(PQgetvalue(res, 0, 0), NULL, 0);

	PQclear(res);

	return retval;

}


uint64_t PostgreSQLPumpCallback::insert_swdesc(const std::string &swdesc)
{
#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Going to insert swdesc: %s.", 
		    swdesc.c_str());
#endif

	int nparams = 1;
	long long int retval;
	const Oid param_types[] = {1043};
	const char *param_values[] = {swdesc.c_str()};
	const int param_lengths[] = {(int) swdesc.length()};
	const int *param_formats = {0};
	const int result_format = 0;
	const char *command =
		"INSERT INTO software (description) \
			VALUES ($1) \
			RETURNING swid;";

	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));

		PQclear(res);

		return -1;

	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Successfully inserted node.");
#endif
	retval = strtoll(PQgetvalue(res, 0, 0), NULL, 0);

	PQclear(res);

	return retval;
}


uint64_t PostgreSQLPumpCallback::insert_indesc(const std::string &indesc)
{
#ifndef NDEBUG
        logger->log(Logger::DEBUG, "Going to insert indesc: %s.", indesc.c_str());
#endif

	long long int retval;
	int nparams = 1;
	const Oid param_types[] = {1043};
	const char *param_values[] = {indesc.c_str()};
	const int param_lengths[] = {(int) indesc.length()};
	const int *param_formats = {0};
	const int result_format = 0;
	const char *command =
		"INSERT INTO instances (description) \
			VALUES ($1) \
			RETURNING iid;";

	PGresult *res = PQexecParams(dbconn_, command, nparams, param_types, 
				     param_values, param_lengths, param_formats, 
				     result_format);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		logger->log(Logger::ERR, "DB insert failed: %s.", 
			    PQresultErrorMessage(res));

		PQclear(res);

		return -1;

	}

#ifndef NDEBUG
	logger->log(Logger::DEBUG, "Successfully inserted node.");
#endif
	retval = strtoll(PQgetvalue(res, 0, 0), NULL, 0);

	PQclear(res);

	return retval;

}


uint64_t PostgreSQLPumpCallback::htonull(const uint64_t n) const
{
	uint64_t retval;

	retval = n;

	retval = ((uint64_t) htonl(n & 0xFFFFFFFF)) << 32;
	retval |= htonl((n & 0xFFFFFFFF00000000) >> 32);

	return retval;
}

