#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>
#include <utility> // for pair.

#include <monitoring/system_information/SystemInformationGlobalIDManagerImplementation.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include "FileBasedSystemInformationOptions.hpp"


using namespace std;

using namespace monitoring;
using namespace core;
using namespace boost;


namespace monitoring {
	class FileBasedSystemInformation: public SystemInformationGlobalIDManager {

			void load( string filename ) {
				//domain_name_map.clear();
				//attribute_map.clear();

				ifstream file( filename );
				if( ! file.good() )
					return;
				try {
					boost::archive::xml_iarchive archive( file, boost::archive::no_header | boost::archive::no_codecvt );
					archive >> boost::serialization::make_nvp( "nextID", nextID );
					archive >> boost::serialization::make_nvp( "valueStringMap", valueStringMap );
					archive >> boost::serialization::make_nvp( "deviceNodeMap", deviceNodeMap );
					archive >> boost::serialization::make_nvp( "interfaceImplStrMap", interfaceImplStrMap );
					archive >> boost::serialization::make_nvp( "interfaceImplMap", interfaceImplMap );
					archive >> boost::serialization::make_nvp( "deviceMap", deviceMap );
					archive >> boost::serialization::make_nvp( "filesystemMap", filesystemMap );
					archive >> boost::serialization::make_nvp( "nodeMap", nodeMap );
					archive >> boost::serialization::make_nvp( "activityInterfaceIDMap", activityInterfaceIDMap );
					archive >> boost::serialization::make_nvp( "activityMap", activityMap );

					loadedNextID = nextID;
				} catch( boost::archive::archive_exception e ) {
					cerr << "Input file " << filename << " is damaged, recreating system info!" << endl;
					//domain_name_map.clear();
					//attribute_map.clear();
					nextID = 1;
				}

				file.close();
			}

			void save( string filename ) {
				if (loadedNextID == nextID) return;
				
				ofstream file( filename );
				boost::archive::xml_oarchive archive( file, boost::archive::no_header | boost::archive::no_codecvt );
				archive << boost::serialization::make_nvp( "nextID", nextID );
				archive << boost::serialization::make_nvp( "valueStringMap", valueStringMap );
				archive << boost::serialization::make_nvp( "deviceNodeMap", deviceNodeMap );
				archive << boost::serialization::make_nvp( "interfaceImplStrMap", interfaceImplStrMap );
				archive << boost::serialization::make_nvp( "interfaceImplMap", interfaceImplMap );
				archive << boost::serialization::make_nvp( "deviceMap", deviceMap );
				archive << boost::serialization::make_nvp( "filesystemMap", filesystemMap );
				archive << boost::serialization::make_nvp( "nodeMap", nodeMap );
				archive << boost::serialization::make_nvp( "activityInterfaceIDMap", activityInterfaceIDMap );
				archive << boost::serialization::make_nvp( "activityMap", activityMap );

				file.close();
			}

			void init() {
				FileBasedSystemInformationOptions & o = getOptions<FileBasedSystemInformationOptions>();
				string filename = o.filename;
				if( filename.length() == 0 ) {
					filename = "system_info.dat";
				}
				//cout << "Initializing ID-mapper from file using " << filename << endl;

				load( filename );
			}

			ComponentOptions * AvailableOptions() {
				return new FileBasedSystemInformationOptions();
			}

			~FileBasedSystemInformation() {				
				FileBasedSystemInformationOptions & o = getOptions<FileBasedSystemInformationOptions>();
				string filename = o.filename;

				save( filename );

				for( auto itr = valueStringMap.begin(); itr != valueStringMap.end(); itr++ ) {
					auto pair = *itr;
					//AttributeWithValues * av = pair.second;
					//delete(av);
				}
			}

			///////////////////////////////////////////////////
#define CHECK(map, srch) \
	auto res = map.find(srch); \
	if (res != map.end()){ \
		return res->second;\
	}else{\
		throw NotFoundError();\
	}

			NodeID                      register_nodeID( const string & hostname ) {
				NodeID id  = nodeMap[hostname];
				if( id != 0 ) {
					return id;
				}

				m.lock();
				id = nextID++;
				valueStringMap[id] = hostname;
				nodeMap[hostname] = id;
				m.unlock();

				return id;
			}

			NodeID                      lookup_nodeID( const string & hostname ) const throw( NotFoundError ) {
				CHECK( nodeMap, hostname )
			}

			const string                lookup_node_hostname( NodeID id ) const throw( NotFoundError ) {
				CHECK( valueStringMap, id )
			}


			DeviceID                    register_deviceID( NodeID id, const  string & local_unique_identifier ) {
				// check if device exists already.
				auto pair_str = pair<uint32_t, string>( id, local_unique_identifier );
				uint32_t device_id = deviceMap[pair_str];

				if( device_id != 0 ) {
					return device_id;
				}

				m.lock();
				device_id = nextID++;
				deviceMap[pair_str] = device_id;
				valueStringMap[device_id] = local_unique_identifier;
				deviceNodeMap[device_id] = id;
				m.unlock();

				return device_id;
			}

			DeviceID                    lookup_deviceID( NodeID id, const  string & local_unique_identifier ) const throw( NotFoundError ) {
				// check if device exists already.
				auto pair_str = pair<uint32_t, string>( id, local_unique_identifier );
				CHECK( deviceMap, pair_str )
			}


			FilesystemID                register_filesystemID( const string & global_unique_identifier ) {
				uint32_t fs_id = filesystemMap[global_unique_identifier];
				if( fs_id != 0 ) {
					return fs_id;
				}

				m.lock();
				fs_id = nextID++;
				filesystemMap[global_unique_identifier] = fs_id;
				valueStringMap[fs_id] = global_unique_identifier;
				m.unlock();

				return fs_id;
			}

			FilesystemID                lookup_filesystemID( const string & global_unique_identifier ) const throw( NotFoundError ) {
				CHECK( filesystemMap, global_unique_identifier )
			}


			UniqueComponentActivityID   register_activityID( UniqueInterfaceID id, const string & name ) {
				auto pair_str = pair<uint32_t, string>( id, name );

				UniqueComponentActivityID aid = activityMap[pair_str];

				if( aid != 0 ) {
					return aid;
				}

				m.lock();
				aid = nextID++;
				activityMap[pair_str] = aid;
				activityInterfaceIDMap[aid] = id;
				valueStringMap[aid] = name;
				m.unlock();
				return aid;
			}

			UniqueComponentActivityID   lookup_activityID( UniqueInterfaceID id, const string & name ) const throw( NotFoundError )  {
				auto pair_str = pair<uint32_t, string>( id, name );

				CHECK( activityMap, pair_str )
			}

			UniqueInterfaceID           lookup_interface_of_activity( UniqueComponentActivityID id ) const throw( NotFoundError ) {
				CHECK( activityInterfaceIDMap, id )
			}


			NodeID                      lookup_node_of_device( DeviceID id ) const throw( NotFoundError ) {
				CHECK( deviceNodeMap, id )
			}


			const string               lookup_device_local_name( DeviceID id ) const throw( NotFoundError ) {
				CHECK( valueStringMap, id )
			}

			const string               lookup_filesystem_name( FilesystemID id ) const throw( NotFoundError ) {
				CHECK( valueStringMap, id )
			}

			const string               lookup_activity_name( UniqueComponentActivityID id ) const throw( NotFoundError ) {
				CHECK( valueStringMap, id )
			}


			UniqueInterfaceID           register_interfaceID( const string & interface, const string & implementation ) {
				// check if the interface and implementation combination exists
				auto pair_str = pair<string, string>( interface, implementation );
				uint32_t cur = interfaceImplMap[pair_str];

				if( cur != 0 ) {
					// interface and impl exists
					return cur;
				}

				// check if interface exists
				m.lock();

				cur = nextID++;

				interfaceImplMap[pair_str] = cur;
				interfaceImplStrMap[cur] = pair_str;

				m.unlock();
				return cur;
			}


			UniqueInterfaceID           lookup_interfaceID( const string & interface, const string & implementation )  const throw( NotFoundError ) {

				// check if the interface and implementation combination exists
				auto pair_str = pair<string, string>( interface, implementation );

				auto res = interfaceImplMap.find( pair_str );
				if( res != interfaceImplMap.end() ) {
					uint32_t cur = res->second;
					return cur;
				} else {
					throw NotFoundError();
				}
			}

			const string               lookup_interface_name( UniqueInterfaceID id ) const throw( NotFoundError )  {
				auto res = interfaceImplStrMap.find( id );
				if( res != interfaceImplStrMap.end() ) {
					return res->second.first;
				} else {
					throw NotFoundError();
				}
			}

			const string               lookup_interface_implementation( UniqueInterfaceID id ) const throw( NotFoundError ) {
				auto res = interfaceImplStrMap.find( id );
				if( res != interfaceImplStrMap.end() ) {
					return res->second.second;
				} else {
					throw NotFoundError();
				}
			}


		private:
			mutex m;

			uint32_t nextID = 1;
			uint32_t loadedNextID = 1;

			map<uint32_t, string> valueStringMap;

			map<DeviceID, NodeID> deviceNodeMap;

			map<uint32_t, pair<string, string>> interfaceImplStrMap;

			map<pair<string, string>, uint32_t> interfaceImplMap;

			map<pair<NodeID, string>, uint32_t> deviceMap;

			map<string, FilesystemID> filesystemMap;

			map<string, NodeID> nodeMap;

			map<UniqueComponentActivityID, UniqueInterfaceID> activityInterfaceIDMap;

			map<pair<uint32_t, string>, uint32_t> activityMap;
	};

}



extern "C" {
	void * MONITORING_SYSTEMINFORMATION_GLOBAL_ID_INSTANCIATOR_NAME()
	{
		return new FileBasedSystemInformation();
	}
}
