#include "PostgreSQLBackend.h"


PostgreSQLBackend::PostgreSQLBackend(const std::string &dbinfo)
{
	dbconn_ = PQconnectdb(dbinfo.c_str());
	
	if (PQstatus(dbconn_) != CONNECTION_OK) {

		logger->log(Logger::ERR, "Error connecting to DB: %s.", 
			    PQerrorMessage(dbconn_));

		PQfinish(dbconn_);

		throw(new PostgreSQLBackendException("Error connecting to the database.")); 
		
	}
	

	logger->log(Logger::NOTICE, "Successfully connected to the database.");

}


PostgreSQLBackend::~PostgreSQLBackend()
{
	PQfinish(dbconn_);
	logger->log(Logger::NOTICE, "Connection to the database terminated.");
}


Callback *PostgreSQLBackend::create_callback()
{
	return new PostgreSQLPumpCallback(*dbconn_);
}

