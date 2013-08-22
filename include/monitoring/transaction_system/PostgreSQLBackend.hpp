#ifndef POSTGRESQL_BACKEND_H
#define POSTGRESQL_BACKEND_H

#include <exception>
#include <string>

#include "libpq-fe.h"

#include <core/logger/SioxLogger.hpp>
#include <monitoring/transaction_system/PostgreSQLPumpCallback.hpp>
#include <monitoring/transaction_system/TransactionBackend.hpp>

class PostgreSQLBackendException
		: public std::exception {
	public:
		PostgreSQLBackendException( const char * err_msg ) : err_msg_( err_msg ) {}
		const char * what() const throw() {
			return err_msg_;
		}
	private:
		const char * err_msg_;
};


class PostgreSQLBackend
		: public TransactionBackend {

	public:
		PostgreSQLBackend();
		~PostgreSQLBackend();
		void init( const std::string & dbinfo );

		Callback * create_callback();

	private:
		PGconn * dbconn_;
};

#endif