#ifndef POSTGRESQL_READER_H
#define POSTGRESQL_READER_H

/* This class is for testing purposes and will be refactored into a new one. */
#include "libpq-fe.h"

#include <core/logger/SioxLogger.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/datatypes/ids.hpp>
#include <misc/Util.hpp>

using namespace monitoring;
using namespace util;


class PostgreSQLReader {
public:
	PostgreSQLReader(const std::string &dbinfo);
	~PostgreSQLReader();
	
	ActivityID *activity_id(uint64_t id);
private:
	PGconn *dbconn_;
};

#endif