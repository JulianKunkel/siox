#ifndef POSTGRESQL_PUMP_CALLBACK_H
#define POSTGRESQL_PUMP_CALLBACK_H

#include "libpq-fe.h"
#include <string>

#include "Callback.h"
#include "SioxLogger.h"

class PostgreSQLPumpCallback
   : public Callback {

public:
	PostgreSQLPumpCallback(PGconn &dbconn);
	
protected:
	void handle_message(ConnectionMessage &msg) const;
	void handle_message(boost::shared_ptr<ConnectionMessage> msg) const;

private:
	PGconn *dbconn_;
	
	uint64_t htonull(const uint64_t n) const;
	void store_node(ConnectionMessage &msg);
	void store_activity(ConnectionMessage &msg);
	void insert_activity(ConnectionMessage &msg) const;
	uint64_t insert_node(uint64_t hwid, uint64_t swid, uint64_t iid);
	uint64_t insert_hwdesc(const std::string &hwdesc);
	uint64_t insert_swdesc(const std::string &swdesc);
	uint64_t insert_indesc(const std::string &indesc);
	
};

#endif