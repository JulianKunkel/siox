#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <utility>

#include <monitoring/system_information/SystemInformationGlobalIDManagerImplementation.hpp>

#include "DatabaseSystemInformationOptions.hpp"


using namespace std;

using namespace monitoring;
using namespace core;
using namespace pqxx;

CREATE_SERIALIZEABLE_CLS( DatabaseSystemInformationOptions )

namespace monitoring {

	#define CHECK(map, srch) \
	auto res = map.find(srch); \
	if (res != map.end()) { \
		return res->second;\
	}

	class DatabaseSystemInformation: public SystemInformationGlobalIDManager {
	public:
		void init() {
			DatabaseInformationOptions & o = getOptions<DatabaseSystemInformationOptions>();

			string hostaddr = o.hostaddr;
	    	int port = o.port;
	    	string user = o.user;
	    	string password = o.password;
	    	string dbname = o.dbname;
	    	
	    	// Connect to database
	    	conn = new connection("host="+hostaddr+" port="+port+" user="+user+" password="+password+" dbname="+dbname);
		}

		void  ~DatabaseSystemInformation() {
			conn->disconnect();
		}

		ComponentOptions * AvailableOptions() {
			return new FileBasedSystemInformationOptions();
		}

		NodeID register_nodeID( const string & hostname ) {
			NodeID id  = nodeMap[hostname];
			if( id != 0 ) {
				return id;
			}

			// TODO: Check if already in Database? What happens when inserted twice? -> http://stackoverflow.com/questions/15710162/conditional-insert-into-statement-in-postgres
			// ^ Recherche: http://stackoverflow.com/questions/1109061/insert-on-duplicate-update-postgresql
			// Mehr: http://stackoverflow.com/questions/4069718/postgres-insert-if-does-not-exist-already
			// TODO: ERROR der nie passieren sollte? Abfangen oder weglassen? Eigene Exception?
			// TODO: Fehlermeldungen weiterhin ignorieren?

	        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
	        work insertAction(*conn, "Insert Transaction");

	        // Perform the insert
	        insertAction.exec("INSERT INTO NodeMapping (Hostname) VALUES ('"+insertAction.esc(hostname)+"')");
	        insertAction.commit();

	        work selectAction(*conn, "Select Transaction");

		    // Perform a select
		    result resultSelect = selectAction.exec(("SELECT NodeID FROM NodeMapping WHERE Hostname='" + selectAction.esc(hostname) + "'"));
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["NodeID"].to(id)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
		        // TODO: ERROR (Sollte nie passieren?)
		        return NULL;
		    }

			
			m.lock();
			valueStringMap[id] = hostname;
			nodeMap[hostname] = id;
			m.unlock();

			return id;
		}	

		NodeID lookup_nodeID( const string & hostname ) const throw( NotFoundError ) {
			CHECK( nodeMap, hostname )

			NodeID id;

	        work selectAction(*conn, "Select Transaction");

		    // Perform a select
		    result resultSelect = selectAction.exec(("SELECT NodeID FROM NodeMapping WHERE Hostname='" + selectAction.esc(hostname) + "'"));
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["NodeID"].to(id)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
				throw NotFoundError();
				return NULL;
		    }
		}

		const string & lookup_node_hostname( NodeID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			NodeID id;

	        work selectAction(*conn, "Select Transaction");
	        string sqlstring;

		    // Perform a select
		    sprintf(sqlstring, "SELECT Hostname FROM NodeMapping WHERE NodeID='%d'", id)
		    result resultSelect = selectAction.exec((sqlstring));
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["NodeID"].to(id)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
				throw NotFoundError();
				return NULL;
		    }			
		}

		DeviceID register_deviceID( NodeID id, const  string & local_unique_identifier ) {
			auto pair_str = pair<uint32_t, string>( id, local_unique_identifier );
			uint32_t device_id = deviceMap[pair_str];

			if( device_id != 0 ) {
				return device_id;
			}

	        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
	        work insertAction(*conn, "Insert Transaction");

	        string sqlstring;
	        sprintf(sqlstring, "INSERT INTO DeviceMapping (UniqueIdentifier, NodeID) VALUES ('"+insertAction.esc(local_unique_identifier)+"', '%d')", id);

	        // Perform the insert
	        insertAction.exec(sqlstring);
	        insertAction.commit();

	        work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT DeviceID FROM DeviceMapping WHERE UniqueIdentifier='" + selectAction.esc(local_unique_identifier) + "' AND NodeID = '%d'", id);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["DeviceID"].to(device_id)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
		        // TODO: ERROR
		        return NULL;
		    }
			
			m.lock();
			deviceMap[pair_str] = device_id;
			valueStringMap[device_id] = local_unique_identifier;
			deviceNodeMap[device_id] = id;
			m.unlock();

			return device_id;
		}

		DeviceID lookup_deviceID( NodeID id, const  string & local_unique_identifier ) const throw( NotFoundError ) {
			// check if device exists already.
			auto pair_str = pair<uint32_t, string>( id, local_unique_identifier );
			CHECK( deviceMap, pair_str )

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT DeviceID FROM DeviceMapping WHERE UniqueIdentifier='" + selectAction.esc(local_unique_identifier) + "' AND NodeID = '%d'", id);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["DeviceID"].to(device_id)) {
		            throw integretyError;
		            return NULL;
		        }
		        return device_id;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}		

		NodeID lookup_node_of_device( DeviceID id ) const throw( NotFoundError ) {
			CHECK( deviceNodeMap, id )

			NodeID node_id;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT NodeID FROM DeviceMapping WHERE UniqueIdentifier='" + selectAction.esc(local_unique_identifier) + "' AND DeviceID = '%d'", id);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["NodeID"].to(node_id)) {
		            throw integretyError;
		            return NULL;
		        }
		        return node_id;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}		

		const string & lookup_device_local_name( DeviceID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			string uid;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT UniqueIdentifier FROM DeviceMapping WHERE DeviceID = '%d'", id);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["UniqueIdentifier"].to(uid)) {
		            throw integretyError;
		            return NULL;
		        }
		        return uid;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}		

		FilesystemID register_filesystemID( const string & global_unique_identifier ) {
			uint32_t fs_id = filesystemMap[global_unique_identifier];
			if( fs_id != 0 ) {
				return fs_id;
			}

	        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
	        work insertAction(*conn, "Insert Transaction");

	        string sqlstring;
	        sprintf(sqlstring, "INSERT INTO FileystemMapping (UniqueIdentifier) VALUES ('"+insertAction.esc(global_unique_identifier)+"')");

	        // Perform the insert
	        insertAction.exec(sqlstring);
	        insertAction.commit();

	        work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT FilesystemID FROM FileystemMapping WHERE UniqueIdentifier='" + selectAction.esc(global_unique_identifier) + "'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["FilesystemID"].to(fs_id)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
		        // TODO: ERROR
		        return NULL;
		    }

			m.lock();
			filesystemMap[global_unique_identifier] = fs_id;
			valueStringMap[fs_id] = global_unique_identifier;
			m.unlock();

			return fs_id;
		}

		FilesystemID lookup_filesystemID( const string & global_unique_identifier ) const throw( NotFoundError ) {
			CHECK( filesystemMap, global_unique_identifier )

			FilesystemID fsid;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT FilesystemID FROM FileystemMapping WHERE UniqueIdentifier = '%s'", global_unique_identifier);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["FilesystemID"].to(fsid)) {
		            throw integretyError;
		            return NULL;
		        }
		        return fsid;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}		

		const string & lookup_filesystem_name( FilesystemID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			string fsname;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT UniqueIdentifier FROM FileystemMapping WHERE FilesystemID = '%d'", id);

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["UniqueIdentifier"].to(fsname)) {
		            throw integretyError;
		            return NULL;
		        }
		        return fsname;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}

		UniqueInterfaceID register_interfaceID( const string & interface, const string & implementation ) {
			// check if the interface and implementation combination exists
			auto pair_str = pair<string, string>( interface, implementation );
			uint32_t cur = interfaceImplMap[pair_str];

			if( cur != 0 ) {
				// interface and impl exists
				return cur;
			}

	        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
	        work insertAction(*conn, "Insert Transaction");

	        string sqlstring;
	        sprintf(sqlstring, "INSERT INTO UniqueInterfaceMapping (Implementation, Interface) VALUES ('"+insertAction.esc(implementation)+"', '"+insertAction.esc(interface)+"')");

	        // Perform the insert
	        insertAction.exec(sqlstring);
	        insertAction.commit();

	        work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT UniqueInterfaceID FROM UniqueInterfaceMapping WHERE Implementation='" + selectAction.esc(implementation) + "' AND Interface = '"+insertAction.esc(interface)+"'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["UniqueInterfaceID"].to(cur)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
		        // TODO: ERROR
		        return NULL;
		    }

			m.lock();
			interfaceImplMap[pair_str] = cur;
			interfaceImplStrMap[cur] = pair_str;

			m.unlock();
			return cur;
		}


		UniqueInterfaceID lookup_interfaceID( const string & interface, const string & implementation )  const throw( NotFoundError ) {
			// check if the interface and implementation combination exists
			auto pair_str = pair<string, string>( interface, implementation );

			auto res = interfaceImplMap.find( pair_str );
			if( res != interfaceImplMap.end() ) {
				uint32_t cur = res->second;
				return cur;
			} else {
				UniqueInterfaceID retInterfaceID;

				work selectAction(*conn, "Select Transaction");
	        	sprintf(sqlstring, "SELECT UniqueInterfaceID FROM UniqueInterfaceMapping WHERE Implementation='" + selectAction.esc(implementation) + "' AND Interface = '"+insertAction.esc(interface)+"'");

		    	// Perform a select
		    	result resultSelect = selectAction.exec(sqlstring);
		    	selectAction.commit();

		    	// Check if there is only one result
		    	if (resultSelect.size() == 1) {
		        	// Check if results are sane (and convert them)
		        	// TODO: Conversion okay?
		        	if (!resultSelect[0]["UniqueInterfaceID"].to(retInterfaceID)) {
		            	throw integretyError;
		            	return NULL;
		        	}
		        	return retInterfaceID;
		    	}
		    	else {
		        	throw NotFoundError();
		        	return NULL;
		    	}
			}
		}

		const string & lookup_interface_name( UniqueInterfaceID id ) const throw( NotFoundError )  {
			auto res = interfaceImplStrMap.find( id );
			if( res != interfaceImplStrMap.end() ) {
				return res->second.first;
			} else {
				string retInterfaceName;

				work selectAction(*conn, "Select Transaction");
	        	sprintf(sqlstring, "SELECT Interface FROM UniqueInterfaceMapping WHERE Implementation='" + selectAction.esc(implementation) + "' AND UniqueInterfaceID = '"+insertAction.esc(id)+"'");

		    	// Perform a select
		    	result resultSelect = selectAction.exec(sqlstring);
		    	selectAction.commit();

		    	// Check if there is only one result
		    	if (resultSelect.size() == 1) {
		        	// Check if results are sane (and convert them)
		        	// TODO: Conversion okay?
		        	if (!resultSelect[0]["Interface"].to(retInterfaceName)) {
		            	throw integretyError;
		            	return NULL;
		        	}
		        	return retInterfaceName;
		    	}
		    	else {
		        	throw NotFoundError();
		        	return NULL;
		    	}
			}
		}

		const string & lookup_interface_implementation( UniqueInterfaceID id ) const throw( NotFoundError ) {
			auto res = interfaceImplStrMap.find( id );
			if( res != interfaceImplStrMap.end() ) {
				return res->second.second;
			} else {
				string retInterfaceImpl;

				work selectAction(*conn, "Select Transaction");
	        	sprintf(sqlstring, "SELECT Implementation FROM UniqueInterfaceMapping WHERE UniqueInterfaceID = '"+insertAction.esc(id)+"'");

		    	// Perform a select
		    	result resultSelect = selectAction.exec(sqlstring);
		    	selectAction.commit();

		    	// Check if there is only one result
		    	if (resultSelect.size() == 1) {
		        	// Check if results are sane (and convert them)
		        	// TODO: Conversion okay?
		        	if (!resultSelect[0]["Implementation"].to(retInterfaceImpl)) {
		            	throw integretyError;
		            	return NULL;
		        	}
		        	return retInterfaceImpl;
		    	}
		    	else {
		        	throw NotFoundError();
		        	return NULL;
		    	}
			}
		}

		UniqueComponentActivityID register_activityID( UniqueInterfaceID id, const string & name ) {
			auto pair_str = pair<uint32_t, string>( id, name );

			UniqueComponentActivityID aid = activityMap[pair_str];

			if( aid != 0 ) {
				return aid;
			}

	        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
	        work insertAction(*conn, "Insert Transaction");

	        string sqlstring;
	        sprintf(sqlstring, "INSERT INTO ComponentActivityMapping (UniqueInterfaceID, Name) VALUES ('"+insertAction.esc(id)+"', '"+insertAction.esc(name)+"')");

	        // Perform the insert
	        insertAction.exec(sqlstring);
	        insertAction.commit();

	        work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT ActivityID FROM ComponentActivityMapping WHERE UniqueInterfaceID='" + selectAction.esc(id) + "' AND Name = '"+insertAction.esc(name)+"'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["ActivityID"].to(aid)) {
		            throw integretyError;
		            return NULL;
		        }
		    }
		    else {
		        // TODO: ERROR
		        return NULL;
		    }

			m.lock();
			activityMap[pair_str] = aid;
			activityInterfaceIDMap[aid] = id;
			valueStringMap[aid] = name;
			m.unlock();
			return aid;
		}

		UniqueComponentActivityID lookup_activityID( UniqueInterfaceID id, const string & name ) const throw( NotFoundError )  {
			auto pair_str = pair<uint32_t, string>( id, name );

			CHECK( activityMap, pair_str )

			UniqueComponentActivityID RetActivityID;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT ActivityID FROM ComponentActivityMapping WHERE UniqueInterfaceID='" + selectAction.esc(id) + "' AND Name = '"+insertAction.esc(name)+"'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["ActivityID"].to(RetActivityID)) {
		            throw integretyError;
		            return NULL;
		        }
		        return RetActivityID;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}

		UniqueInterfaceID lookup_interface_of_activity( UniqueComponentActivityID id ) const throw( NotFoundError ) {
			CHECK( activityInterfaceIDMap, id )

			UniqueInterfaceID RetInterfaceID;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT UniqueInterfaceID FROM ComponentActivityMapping WHERE ActivityID='" + selectAction.esc(id) + "'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["UniqueInterfaceID"].to(RetInterfaceID)) {
		            throw integretyError;
		            return NULL;
		        }
		        return RetInterfaceID;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}

		const string & lookup_activity_name( UniqueComponentActivityID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			string RetActName;

			work selectAction(*conn, "Select Transaction");
	        sprintf(sqlstring, "SELECT Name FROM ComponentActivityMapping WHERE UniqueInterfaceID='" + selectAction.esc(id) + "'");

		    // Perform a select
		    result resultSelect = selectAction.exec(sqlstring);
		    selectAction.commit();

		    // Check if there is only one result
		    if (resultSelect.size() == 1) {
		        // Check if results are sane (and convert them)
		        // TODO: Conversion okay?
		        if (!resultSelect[0]["Name"].to(RetActName)) {
		            throw integretyError;
		            return NULL;
		        }
		        return RetActName;
		    }
		    else {
		        throw NotFoundError();
		        return NULL;
		    }
		}		


	private:
		connection* conn;

		mutex m;
		map<string, NodeID> nodeMap;
		map<uint32_t, string> valueStringMap;	

	};
}