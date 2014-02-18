#include <monitoring/activity_multiplexer/plugins/bdbwriter/ActivityBDBWriter.hpp>

#include <monitoring/activity_multiplexer/plugins/bdbwriter/ActivityBDBWriterPluginOptions.hpp>

#include <monitoring/activity_multiplexer/plugins/bdbwriter/ActivityBinarySerializable.hpp>

void ActivityBDBWriterPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	uint64_t pos = 0;
	char buff[30];

	Activity * act = &* activity;

	DBT rkey, rdata;
	memset(& rkey, 0, sizeof(DBT));
	memset(& rdata, 0, sizeof(DBT));

	rkey.size = j_serialization::serializeLen( act->aid_ );
	rkey.data = buff;
	j_serialization::serialize(act->aid_, buff, pos);
	assert( pos <= sizeof(buff) );

	rdata.size = j_serialization::serializeLen(*act);
	rdata.data = malloc(rdata.size);
	pos = 0;
	j_serialization::serialize(* act, (char*) rdata.data, pos);

	int ret = dbp->put(dbp, NULL, & rkey, & rdata, DB_NOOVERWRITE);
	if (ret == DB_KEYEXIST) {
   	dbp->err(dbp, ret, "Put failed because key already exists");
	}
	free(rdata.data);
}

ComponentOptions * ActivityBDBWriterPlugin::AvailableOptions() {
	return new ActivityBDBWriterPluginOptions();
}

void ActivityBDBWriterPlugin::initPlugin( ) {
	ActivityBDBWriterPluginOptions & o = getOptions<ActivityBDBWriterPluginOptions>();

	int ret;

	struct stat sb;
	if (stat(o.dirname.c_str(), &sb) != 0){
		if (mkdir(o.dirname.c_str(), S_IRWXU) != 0) { 
			fprintf(stderr, "DB error mkdir: %s: %s\n", o.dirname.c_str(), strerror(errno)); 
			exit (1);
		}
	}

	if ((ret = db_env_create(& dbenv, 0)) != 0) {
		fprintf(stderr, "DB error db_env_create: %s\n", db_strerror(ret));
		exit (1);
	}

	dbenv->set_errpfx(dbenv, __FILE__);

	if ((ret = dbenv->set_cache_max(dbenv, 0, 10*1024*1024)) != 0){
		dbenv->err(dbenv, ret, "set_cachesize");
		exit (1);
	}

	if ((ret = dbenv->set_cachesize(dbenv, 0, 10*1024*1024, 0)) != 0){
		dbenv->err(dbenv, ret, "set_cachesize");
		exit (1);
	}

	if ((ret = dbenv->open(dbenv, o.dirname.c_str(), DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG |
	    DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_THREAD,  S_IRUSR | S_IWUSR)) != 0) {
		dbenv->close(dbenv, 0);
		dbenv->err(dbenv, ret, "dbenv->open: %s", o.dirname.c_str());
		exit (1);
	}

	if ((ret = dbenv->set_flags(dbenv, (o.durable ? ( DB_AUTO_COMMIT | ( o.synchronize ? 0 : DB_TXN_NOSYNC)) : 0) , 1 )) != 0 ){
		dbenv->err(dbenv, ret, "set_flags");
		exit (1);
	}

	if ((ret = db_create(& dbp, dbenv, 0)) != 0) {
		dbenv->err(dbenv, ret, "db_create");
		exit (1);
	}

	/* open the database */
	if ((ret = dbp->open(dbp,        
	                NULL,       
	                "actvities.db", 
	                NULL,       
	                DB_BTREE,   /* Database access method */
	                DB_CREATE | DB_THREAD,      /* Open flags */
	                S_IRUSR | S_IWUSR) /* File mode */
	                ) != 0){
		dbenv->err(dbenv, ret, "db_create");
		exit (1);
	}

	multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( & ActivityBDBWriterPlugin::Notify ), false );
}

void ActivityBDBWriterPlugin::finalize() {
	multiplexer->unregisterCatchall( this, false );
	ActivityMultiplexerPlugin::finalize();
}

ActivityBDBWriterPlugin::~ActivityBDBWriterPlugin() {
	if ( dbp != nullptr ){
		dbp->close(dbp, 0); 
		dbenv->close(dbenv, 0);
	}
		}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityBDBWriterPlugin();
	}
}
