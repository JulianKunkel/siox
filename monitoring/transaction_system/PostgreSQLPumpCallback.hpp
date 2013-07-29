#ifndef POSTGRESQL_PUMP_CALLBACK_H
#define POSTGRESQL_PUMP_CALLBACK_H

#include "libpq-fe.h"
#include <string>

#include "Activity.hpp"
#include "Callback.hpp"
#include "SioxLogger.hpp"

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
	
};

#endif