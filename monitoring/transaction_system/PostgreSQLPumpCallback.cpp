#include <arpa/inet.h>
#include <boost/ptr_container/ptr_list.hpp>
#include <iostream>
#include <sstream>
#include <string>

/**
 * @todo Use the param_types vector.
 *
 */

#include <monitoring/transaction_system/PostgreSQLPumpCallback.hpp>
#include <misc/Util.hpp>

PostgreSQLPumpCallback::PostgreSQLPumpCallback( PGconn & dbconn )
	: dbconn_( &dbconn )
{
}


void PostgreSQLPumpCallback::handle_message( ConnectionMessage & msg ) const
{
	if( msg.get_msg()->action() == buffers::MessageBuffer::Activity ) {

		store_activity( msg );

	} else if( msg.get_msg()->action() == buffers::MessageBuffer::RegisterNode ) {

		store_node( msg );

	}
}


void PostgreSQLPumpCallback::handle_message( boost::shared_ptr<ConnectionMessage> msg ) const
{
}


void PostgreSQLPumpCallback::store_node( ConnectionMessage & msg ) const
{
#ifndef NDEBUG
	logger->log( Logger::NOTICE, "Prepared to insert node." );
#endif
	const buffers::Node & node = msg.get_msg()->node();

	store_node( node.hwdesc(), node.swdesc(), node.indesc() );

#ifndef NDEBUG
//      logger->log(Logger::NOTICE, "Inserted: %" PRIx64 " %" PRIx64 "  %" PRIx64 ".", hwid, swid, iid);
#endif
}


void PostgreSQLPumpCallback::store_node( const std::string & hwdesc,
        const std::string & swdesc,
        const std::string & indesc ) const
{
	uint64_t hwid = insert_hwdesc( hwdesc );
	uint64_t swid = insert_swdesc( swdesc );
	uint64_t  iid = insert_indesc( indesc );

	insert_node( hwid, swid, iid );
}


uint64_t PostgreSQLPumpCallback::insert_hwdesc( const std::string & hwdesc ) const
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Going to insert hwdesc: %s.\n",
	             hwdesc.c_str() );
#endif

	int nparams = 1;
	long long int retval;
	const Oid param_types[] = {1043};
	const char * param_values[] = {hwdesc.c_str()};
	const int param_lengths[] = {( int ) hwdesc.length()};
	const int * param_formats = {0};
	const int result_format = 0;
	const char * command =
	    "INSERT INTO hardware (description) \
			VALUES ($1) \
			RETURNING id;";

	PGresult * res = PQexecParams( dbconn_, command, nparams, param_types,
	                               param_values, param_lengths, param_formats,
	                               result_format );

	if( PQresultStatus( res ) != PGRES_TUPLES_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.\n",
		             PQresultErrorMessage( res ) );

		PQclear( res );

		return -1;

	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Successfully inserted hardware." );
#endif

	retval = strtoll( PQgetvalue( res, 0, 0 ), NULL, 0 );

	PQclear( res );

	return retval;

}


uint64_t PostgreSQLPumpCallback::insert_swdesc( const std::string & swdesc ) const
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Going to insert swdesc: %s.\n",
	             swdesc.c_str() );
#endif

	int nparams = 1;
	long long int retval;
	const Oid param_types[] = {1043};
	const char * param_values[] = {swdesc.c_str()};
	const int param_lengths[] = {( int ) swdesc.length()};
	const int * param_formats = {0};
	const int result_format = 0;
	const char * command =
	    "INSERT INTO software (description) \
			VALUES ($1) \
			RETURNING id;";

	PGresult * res = PQexecParams( dbconn_, command, nparams, param_types,
	                               param_values, param_lengths, param_formats,
	                               result_format );

	if( PQresultStatus( res ) != PGRES_TUPLES_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.",
		             PQresultErrorMessage( res ) );

		PQclear( res );

		return -1;

	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Successfully inserted software." );
#endif
	retval = strtoll( PQgetvalue( res, 0, 0 ), NULL, 0 );

	PQclear( res );

	return retval;
}


uint64_t PostgreSQLPumpCallback::insert_indesc( const std::string & indesc ) const
{
#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Going to insert indesc: %s.\n",
	             indesc.c_str() );
#endif

	long long int retval;
	int nparams = 1;
	const Oid param_types[] = {1043};
	const char * param_values[] = {indesc.c_str()};
	const int param_lengths[] = {( int ) indesc.length()};
	const int * param_formats = {0};
	const int result_format = 0;
	const char * command =
	    "INSERT INTO instance (description) \
			VALUES ($1) \
			RETURNING id;";

	PGresult * res = PQexecParams( dbconn_, command, nparams, param_types,
	                               param_values, param_lengths, param_formats,
	                               result_format );

	if( PQresultStatus( res ) != PGRES_TUPLES_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.",
		             PQresultErrorMessage( res ) );

		PQclear( res );

		return -1;

	}

#ifndef NDEBUG
	logger->log( Logger::DEBUG, "Successfully inserted instance." );
#endif
	retval = strtoll( PQgetvalue( res, 0, 0 ), NULL, 0 );

	PQclear( res );

	return retval;

}


uint64_t PostgreSQLPumpCallback::insert_node( uint64_t hwid, uint64_t swid,
        uint64_t iid ) const
{
#ifndef NDEBUG
//  logger->log(Logger::DEBUG, "Going to insert node (%" PRIu64 ", %"
//          PRIu64 ", %" PRIu64 ").", hwid, swid, iid);
#endif

	hwid = util::htonll( hwid );
	swid = util::htonll( swid );
	iid  = util::htonll( iid );

	int nparams = 3;
	const Oid param_types[3] = {20, 20, 20};
	const char * param_values[3] = {
		( char * ) & hwid,
		( char * ) & swid,
		( char * ) & iid
	};
	const int param_lengths[3] = {
		sizeof( hwid ),
		sizeof( swid ),
		sizeof( iid )
	};
	const int param_formats[3] = {1, 1, 1};
	const int result_format = 0;
	const char * command =
	    "INSERT INTO node (hwid, swid, iid) \
			VALUES ($1::int8, $2::int8, $3::int8) \
			RETURNING id;";

	PGresult * res = PQexecParams( dbconn_, command, nparams, param_types,
	                               param_values, param_lengths, param_formats,
	                               result_format );

	if( PQresultStatus( res ) != PGRES_TUPLES_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.",
		             PQresultErrorMessage( res ) );

		PQclear( res );

		return -1;

	}

	uint64_t retval = strtoll( PQgetvalue( res, 0, 0 ), NULL, 0 );

#ifndef NDEBUG
	logger->log( Logger::NOTICE, "Successfully inserted node." );
#endif
	PQclear( res );

	return retval;

}


void PostgreSQLPumpCallback::store_activity( ConnectionMessage & msg ) const
{
	const buffers::Activity & act = msg.get_msg()->activity();

}


void PostgreSQLPumpCallback::store_activity( const Activity & act ) const
{
	uint64_t unique_aid = insert_activity( act );

	if( !act.parentArray().empty() ) {

		std::vector<ActivityID>::const_iterator i;
		const vector<ActivityID> parentArray = act.parentArray();

		for( i = parentArray.begin(); i < parentArray.end(); ++i ) {
			insert_parent( ( *i ), unique_aid );
		}

	}

}


std::string
PostgreSQLPumpCallback::serialize_attributes( const Activity & act ) const
{
	std::stringstream attr_string;

	if( act.attributeArray().empty() )
		return attr_string.str();

	attr_string << "{";

	const vector<Attribute> attributeArray = act.attributeArray();
	std::vector<Attribute>::const_iterator i;

	for( i = attributeArray.begin(); i < attributeArray.end(); ++i ) {

		attr_string << "\"(";
		attr_string << ( *i ).id << ",";
		attr_string << ( *i ).value;
		attr_string << ")\"";

		if( i != attributeArray.end() - 1 )
			attr_string << ", ";

	}

	attr_string << "}";

	return attr_string.str();
}


uint64_t PostgreSQLPumpCallback::insert_activity( const Activity & act ) const
{
	const std::string attributes = serialize_attributes( act );

	uint32_t       aid_id = htonl( act.aid().id );
	uint32_t  aid_cid_nid = htonl( act.aid().cid.pid.nid );
	uint32_t  aid_cid_pid = htonl( act.aid().cid.pid.pid );
	uint32_t aid_cid_time = htonl( act.aid().cid.pid.time );
	uint32_t  aid_cid_num = htonl( act.aid().cid.id );

	uint32_t       ucaid = htonl( act.ucaid() );
	uint64_t  time_start = util::htonll( act.time_start() );
	uint64_t   time_stop = util::htonll( act.time_stop() );
	uint32_t error_value = htonl( act.errorValue() );

	const int nparams = 10;
	const char * param_values[nparams] = {
		( char * ) & aid_id,
		( char * ) & aid_cid_nid,
		( char * ) & aid_cid_pid,
		( char * ) & aid_cid_time,
		( char * ) & aid_cid_num,
		( char * ) & ucaid,
		( char * ) & time_start,
		time_stop   == 0 ? NULL : ( char * ) & time_stop,
		error_value == 0 ? NULL : ( char * ) & error_value,
		attributes.length() == 0 ? NULL : attributes.c_str()
	};

	const int param_lengths[nparams] = {
		sizeof( aid_id ),
		sizeof( aid_cid_nid ),
		sizeof( aid_cid_pid ),
		sizeof( aid_cid_time ),
		sizeof( aid_cid_num ),
		sizeof( ucaid ),
		sizeof( time_start ),
		sizeof( time_stop ),
		sizeof( error_value ),
		( int ) attributes.length()
	};

	const Oid param_types[nparams] = {20, 20, 20, 20, 23, 20, 20, 20, 20, 1043};
	const int param_formats[nparams] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
	const int result_format = 0;

	const char * command =
	    "INSERT INTO activity (                                        \
			aid_id,                                                \
			aid_cid_nid, aid_cid_pid, aid_cid_time, aid_cid_num,   \
			ucaid, time_start, time_stop, error_value,             \
			attributes                                             \
		) VALUES (                                                     \
			$1::int4,                                              \
			$2::int4, $3::int4, $4::int4, $5::int4,                \
			$6::int4, $7::int8, $8::int8, $9::int4,                \
			$10                                                    \
		) RETURNING unique_id;";

	PGresult * res = PQexecParams( dbconn_, command, nparams, NULL,
	                               param_values, param_lengths, param_formats,
	                               result_format );


	if( PQresultStatus( res ) != PGRES_TUPLES_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.",
		             PQresultErrorMessage( res ) );

		PQclear( res );

		return 0;

	}

	uint64_t retval = strtoll( PQgetvalue( res, 0, 0 ), NULL, 0 );

#ifndef NDEBUG
	logger->log( Logger::NOTICE, "Successfully inserted core activity." );
#endif
	PQclear( res );

	return retval;

}


void PostgreSQLPumpCallback::insert_parent( const ActivityID & parent,
        uint64_t child_id ) const
{
	uint32_t       p_id = htonl( parent.id );
	uint32_t  p_cid_nid = htonl( parent.cid.pid.nid );
	uint32_t  p_cid_pid = htonl( parent.cid.pid.pid );
	uint32_t p_cid_time = htonl( parent.cid.pid.time );
	uint32_t  p_cid_num = htonl( parent.cid.id );

	child_id = util::htonll( child_id );

	int nparams = 6;
	const char * param_values[6] = {
		( char * ) & p_id,
		( char * ) & p_cid_nid,
		( char * ) & p_cid_pid,
		( char * ) & p_cid_time,
		( char * ) & p_cid_num,
		( char * ) & child_id
	};

	const int param_lengths[6] = {
		sizeof( p_id ),
		sizeof( p_cid_nid ),
		sizeof( p_cid_pid ),
		sizeof( p_cid_time ),
		sizeof( p_cid_num ),
		sizeof( child_id )
	};

	const Oid param_types[6] = {20, 20, 20, 20, 20, 20};
	const int param_formats[6] = {1, 1, 1, 1, 1, 1};
	const int result_format = 0;

	const char * command =
	    "INSERT INTO parent (                                          \
			parent_aid_id,                                         \
			parent_cid_nid, parent_cid_pid, parent_cid_time,       \
			parent_cid_num, child_unique_id                        \
		) VALUES (                                                     \
			$1::int4,                                              \
			$2::int4, $3::int4, $4::int4,                          \
			$5::int4, $6::int8                                     \
		);";

	PGresult * res = PQexecParams( dbconn_, command, nparams, NULL,
	                               param_values, param_lengths, param_formats,
	                               result_format );

	if( PQresultStatus( res ) != PGRES_COMMAND_OK ) {

		logger->log( Logger::ERR, "DB insert failed: %s.",
		             PQresultErrorMessage( res ) );

	}

	PQclear( res );

}

