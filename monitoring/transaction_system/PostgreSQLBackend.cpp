#include <monitoring/transaction_system/PostgreSQLBackend.hpp>


PostgreSQLBackend::PostgreSQLBackend()
{
}

void PostgreSQLBackend::init(const std::string &dbinfo)
{
	dbconn_ = PQconnectdb(dbinfo.c_str());

	if (PQstatus(dbconn_) != CONNECTION_OK) {

		logger->log(Logger::ERR, "Error connecting to DB: %s.", PQerrorMessage(dbconn_));

		PQfinish(dbconn_);

		throw (new PostgreSQLBackendException("Error connecting to the database."));

	}

#ifndef NDEBUG
	logger->log(Logger::NOTICE, "Successfully connected to the database.");
#endif
}


PostgreSQLBackend::~PostgreSQLBackend()
{
	PQfinish(dbconn_);

#ifndef NDEBUG
	logger->log(Logger::NOTICE, "Connection to the database terminated.");
#endif
}


Callback *PostgreSQLBackend::create_callback()
{
	return new PostgreSQLPumpCallback(*dbconn_);
}


extern "C" {
	void * get_instance_psql_backend()
	{
		return new PostgreSQLBackend();
	}
}
