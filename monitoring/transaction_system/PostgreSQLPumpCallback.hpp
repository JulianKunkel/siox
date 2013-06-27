#ifndef POSTGRESQL_PUMP_CALLBACK_H
#define POSTGRESQL_PUMP_CALLBACK_H

#include "libpq-fe.h"
#include <string>

#include "Activity.h"
#include "Callback.h"
#include "Component.h"
#include "SioxLogger.h"

using namespace monitoring;

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
	void store_component(ConnectionMessage &msg) const;
	void store_activity(ConnectionMessage &msg) const;
	void insert_activity(Activity &act) const;
	uint64_t insert_component(Component &c) const;
	uint64_t insert_hwdesc(const std::string &hwdesc) const;
	uint64_t insert_swdesc(const std::string &swdesc) const;
	uint64_t insert_indesc(const std::string &indesc) const;
	
};

#endif