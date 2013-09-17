#ifndef POSTGRESQL_PUMP_CALLBACK_HPP
#define POSTGRESQL_PUMP_CALLBACK_HPP

/**
 * This class is responsible for storing the different objects into a PostgreSQL
 * database.
 *
 * @author Alvaro Aguilera
 */

#include "libpq-fe.h"

#include <iostream>
#include <sstream>
#include <string>

#include <monitoring/datatypes/Activity.hpp>
#include <core/comm/modules/asio/Callback.hpp>
#include <core/logger/SioxLogger.hpp>

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

using namespace monitoring;

class PostgreSQLPumpCallbackTest;

class PostgreSQLPumpCallback : public Callback {

friend class PostgreSQLPumpCallbackTest;

public:
	PostgreSQLPumpCallback(PGconn &dbconn);

protected:
// 	void handle_message(ConnectionMessage &msg) const;
// 	void handle_message(boost::shared_ptr<ConnectionMessage> msg) const;

private:
	PGconn *dbconn_;

	void store_activity(Activity &act) const;
	uint64_t insert_activity_id(const ActivityID &aid) const;
	
// 	void store_node(ConnectionMessage &msg) const;
// 	void store_node(const std::string &hwdesc, const std::string &swdesc, const std::string &indesc) const;
// 
// 	uint64_t insert_node(uint64_t hwid, uint64_t swid, uint64_t iid) const;
// 	uint64_t insert_hwdesc(const std::string &hwdesc) const;
// 	uint64_t insert_swdesc(const std::string &swdesc) const;
// 	uint64_t insert_indesc(const std::string &indesc) const;
// 
// 	void store_activity(ConnectionMessage &msg) const;
// 	void store_activity(const Activity &act) const;
// 	uint64_t insert_activity(const Activity &act) const;
// 	void insert_parent(const ActivityID &parent, uint64_t child_id) const;
// 	std::string serialize_attributes(const Activity &act) const;

};

#endif