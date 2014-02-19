#include <db.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

// gcc -g -Wall db.c  -I /opt/siox/bdb/include/  -L  /opt/siox/bdb/lib/  -ldb  -std=gnu99

#define ENV_DIRECTORY "db/"

/*
SSD:
With transactions, AutoCommit: ca. 300 /s
With transactions and DB_TXN_NOSYNC flag: 11,000/s
Without transactions 130,000/s

With transactions and DB_TXN_NOSYNC flag AND a cachesize of 4 MiB: 81,553/s
With transactions and DB_TXN_NOSYNC flag AND a cachesize of 10 MiB: 100,000/s

Shmem:
With transactions and DB_TXN_NOSYNC flag: 105,000/s
*/

uint64_t gettime()
{
        struct timespec tp;
        if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
                return -1;
        } else {
                return ( tp.tv_sec * 1000000000ull ) + tp.tv_nsec;
        }
}


int main(){

	DB *dbp;           /* DB structure handle */
	u_int32_t flags;   /* database open flags */
	int ret;           /* function return value */

	DB_ENV *dbenv;

	struct stat sb;
	if (stat(ENV_DIRECTORY, &sb) != 0){
		if (mkdir(ENV_DIRECTORY, S_IRWXU) != 0) { 
			fprintf(stderr, "DB error mkdir: %s: %s\n", ENV_DIRECTORY, strerror(errno)); 
			exit (1);
		}
	}

	if ((ret = db_env_create(& dbenv, 0)) != 0) {
		fprintf(stderr, "DB error db_env_create: %s\n", db_strerror(ret));
		exit (1);
	}

	dbenv->set_errpfx(dbenv, "DB");

	if ((ret = dbenv->set_cache_max(dbenv, 0, 10*1024*1024)) != 0){
		dbenv->err(dbenv, ret, "set_cachesize");
		return (1);
	}

	if ((ret = dbenv->set_cachesize(dbenv, 0, 10*1024*1024, 0)) != 0){
		dbenv->err(dbenv, ret, "set_cachesize");
		return (1);
	}

	if ((ret = dbenv->open(dbenv, ENV_DIRECTORY, DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG |
	    DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_THREAD,  S_IRUSR | S_IWUSR)) != 0) {
		dbenv->close(dbenv, 0);
		dbenv->err(dbenv, ret, "dbenv->open: %s", ENV_DIRECTORY);
		exit (1);
	}

	

	if ((ret = dbenv->set_flags(dbenv, DB_AUTO_COMMIT | DB_TXN_NOSYNC, 1)) != 0 ){
		dbenv->err(dbenv, ret, "set_flags");
		return (1);
	}
	// http://sepp.oetiker.ch/db-4.2.52-mo/api_c/env_set_flags.html#DB_TXN_WRITE_NOSYNC

	if ((ret = db_create(& dbp, dbenv, 0)) != 0) {
		dbenv->err(dbenv, ret, "db_create");
		return (1);
	}


	/* Database open flags */
	flags = DB_CREATE ; 

	/* open the database */
	ret = dbp->open(dbp,        /* DB structure pointer */
	                NULL,       /* Transaction pointer */
	                "test.db", /* On-disk file that holds the database. */
	                NULL,       /* Optional logical database name */
	                DB_BTREE,   /* Database access method */
	                flags,      /* Open flags */
	                S_IRUSR | S_IWUSR); /* File mode */
	if (ret != 0) {
	  /* Error handling goes here */
	}

	DBT key, data;
	char *description = "DATA T";

	/* Zero out the DBTs before using them. */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.size = sizeof(int);

	data.data = description;
	data.size = strlen(description) + 1; 

   	uint64_t t0 = gettime();
	for(int i=0; i < 10000000; i++){
		key.data = & i;
		ret = dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE);
		if (ret == DB_KEYEXIST) {
		    dbp->err(dbp, ret, "Put failed because key %d already exists", i);
		}

		uint64_t delta = gettime() - t0;
		if ( delta >= 1000000000ull ){
			printf("%d in %lld s => %f/s\n", i, delta/1000000000ull, i/(delta/1000000000.0));
			break;
		}
	}


	DBT rkey, rdata;
	memset(& rkey, 0, sizeof(DBT));
	memset(& rdata, 0, sizeof(DBT));

	int v = 45;
	rkey.data = & v;
	rkey.size = sizeof(int);	

	rdata.flags = DB_DBT_MALLOC; // DB_DBT_USERMEM

	ret = dbp->get(dbp, NULL, & rkey, & rdata, 0);

	printf("Retrieved %s\n", (char *) rdata.data);


   dbp->close(dbp, 0); 


   dbenv->close(dbenv, 0);

   return 0;
}