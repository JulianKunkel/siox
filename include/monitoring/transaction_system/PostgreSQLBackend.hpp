#ifndef POSTGRESQL_BACKEND_H
#define POSTGRESQL_BACKEND_H

#include <exception>
#include <string>

#include "libpq-fe.h"

#include "PostgreSQLPumpCallback.hpp"
#include "SioxLogger.hpp"
#include "TransactionBackend.hpp"

class PostgreSQLBackendException 
   : public std::exception {
public:
	PostgreSQLBackendException(const char *err_msg) : err_msg_(err_msg) {}
	const char *what() const throw() { return err_msg_; }
private:
	const char *err_msg_;
};


class PostgreSQLBackend
   : public TransactionBackend {
	   
public:   
	PostgreSQLBackend(const std::string &dbinfo);
	~PostgreSQLBackend();
	
	Callback *create_callback();
	
private:
	PGconn *dbconn_;
};
   
#endif