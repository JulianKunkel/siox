#ifndef SIOX_CORE_DB_DATABSE_SETUP_HPP
#define SIOX_CORE_DB_DATABSE_SETUP_HPP

class DatabaseSetup{
public:
	/*
	Setup the database etc. 
	 */
	virtual void prepareDatabaseIfNecessary() = 0;

	/*
	Empty the database, this function must be callable regardless of the connection status with the DB.
	*/
	virtual void cleanDatabase() = 0;
};

#endif