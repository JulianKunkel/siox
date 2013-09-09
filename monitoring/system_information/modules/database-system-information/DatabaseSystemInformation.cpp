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
	} else {\
		throw NotFoundError();\
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
	    	conn = new connection("hostaddr="+hostaddr+" port="+port+" user="+user+" password="+password+" dbname="+dbname);
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

			// TODO: id aus der Datenbank holen und füllen
			
			m.lock();
			valueStringMap[id] = hostname;
			nodeMap[hostname] = id;
			m.unlock();

			return id;
		}	

		NodeID lookup_nodeID( const string & hostname ) const throw( NotFoundError ) {
			CHECK( nodeMap, hostname )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		const string & lookup_node_hostname( NodeID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		DeviceID register_deviceID( NodeID id, const  string & local_unique_identifier ) {
			auto pair_str = pair<uint32_t, string>( id, local_unique_identifier );
			uint32_t device_id = deviceMap[pair_str];

			if( device_id != 0 ) {
				return device_id;
			}

			// TODO: Datenbank query'en
			m.lock();
			device_id = nextID++;
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

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}		

		NodeID lookup_node_of_device( DeviceID id ) const throw( NotFoundError ) {
			CHECK( deviceNodeMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}		

		const string & lookup_device_local_name( DeviceID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}		

		FilesystemID register_filesystemID( const string & global_unique_identifier ) {
			uint32_t fs_id = filesystemMap[global_unique_identifier];
			if( fs_id != 0 ) {
				return fs_id;
			}

			// TODO: Datenbank
			m.lock();
			fs_id = nextID++;
			filesystemMap[global_unique_identifier] = fs_id;
			valueStringMap[fs_id] = global_unique_identifier;
			m.unlock();

			return fs_id;
		}

		FilesystemID lookup_filesystemID( const string & global_unique_identifier ) const throw( NotFoundError ) {
			CHECK( filesystemMap, global_unique_identifier )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}		

		const string & lookup_filesystem_name( FilesystemID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		UniqueInterfaceID register_interfaceID( const string & interface, const string & implementation ) {
			// check if the interface and implementation combination exists
			auto pair_str = pair<string, string>( interface, implementation );
			uint32_t cur = interfaceImplMap[pair_str];

			if( cur != 0 ) {
				// interface and impl exists
				return cur;
			}

			// check if interface exists

			// TODO: Datenbank
			m.lock();

			cur = nextID++;

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
				throw NotFoundError();
			}

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		const string & lookup_interface_name( UniqueInterfaceID id ) const throw( NotFoundError )  {
			auto res = interfaceImplStrMap.find( id );
			if( res != interfaceImplStrMap.end() ) {
				return res->second.first;
			} else {
				throw NotFoundError();
			}

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		const string & lookup_interface_implementation( UniqueInterfaceID id ) const throw( NotFoundError ) {
			auto res = interfaceImplStrMap.find( id );
			if( res != interfaceImplStrMap.end() ) {
				return res->second.second;
			} else {
				throw NotFoundError();
			}

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		UniqueComponentActivityID register_activityID( UniqueInterfaceID id, const string & name ) {
			auto pair_str = pair<uint32_t, string>( id, name );

			UniqueComponentActivityID aid = activityMap[pair_str];

			if( aid != 0 ) {
				return aid;
			}


			// TODO: Datenbank
			m.lock();
			aid = nextID++;
			activityMap[pair_str] = aid;
			activityInterfaceIDMap[aid] = id;
			valueStringMap[aid] = name;
			m.unlock();
			return aid;
		}

		UniqueComponentActivityID lookup_activityID( UniqueInterfaceID id, const string & name ) const throw( NotFoundError )  {
			auto pair_str = pair<uint32_t, string>( id, name );

			CHECK( activityMap, pair_str )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		UniqueInterfaceID lookup_interface_of_activity( UniqueComponentActivityID id ) const throw( NotFoundError ) {
			CHECK( activityInterfaceIDMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}

		const string & lookup_activity_name( UniqueComponentActivityID id ) const throw( NotFoundError ) {
			CHECK( valueStringMap, id )

			// TODO: Falls CHECK fehlschlägt: Datenbank abfragen, dann erst exception
		}		


	private:
		connection* conn;

		mutex m;
		map<string, NodeID> nodeMap;
		map<uint32_t, string> valueStringMap;	

	};
}