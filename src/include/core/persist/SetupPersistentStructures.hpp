#ifndef SIOX_CORE_PERSIST_SETUP_PERSISTENT_STRUCTURES_HPP
#define SIOX_CORE_PERSIST_SETUP_PERSISTENT_STRUCTURES_HPP

/*
 Plugins shall inherit from this class if they are used to store some persistent information.
 With the right flags the daemon invokes these methods and thus folders and databases can be setup 
 to be ready for later use.
 */
class SetupPersistentStructures{
public:
	/*
	Setup the database etc. 
	 */
	virtual int preparePersistentStructuresIfNecessary() = 0;

	/*
	Empty the database, this function must be callable regardless of the connection status with the DB.
	*/
	virtual int cleanPersistentStructures() = 0;
};

#endif