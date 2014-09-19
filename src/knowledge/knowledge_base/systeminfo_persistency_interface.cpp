#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <knowledge/knowledge_base/systeminfo_persistency_interface.hpp>

// No conflicts between std and pqxx
using namespace std;
using namespace pqxx;

namespace knowledge {

	/*
	 On SQL-Injections

	 pqxx's work-object has a method called "esc", which takes and returns a string.
	 Use it on _any_ user-changeable string before pushing it into the database.
	*/

// Exception for things that shouldn't go wrong (and thus arn't addresses by the databases exception handling)
	class integrityException: public exception {
			virtual const char * what() const throw() {
				return "Database returned something unexpected.";
			}
	} integretyError;

	void DBLayer::init( ComponentOptions * options )
	{
		ComponentOptions * myOpts = options;

		list<ComponentOptionEntry>* myList;
		myList = new list<ComponentOptionEntry>;

		string hostaddr;
		int port;
		string user;
		string password;
		string dbname;

		myList->push_back( new ComponentOptionEntry( "hostaddress", typeid( string ), &hostaddr ) );
		myList->push_back( new ComponentOptionEntry( "port", typeid( int ), &port ) );
		myList->push_back( new ComponentOptionEntry( "user", typeid( string ), &user ) );
		myList->push_back( new ComponentOptionEntry( "password", typeid( string ), &password ) );
		myList->push_back( new ComponentOptionEntry( "dbname", typeid( string ), &dbname ) );

		options->get_component_options( myList );

		conn = new connection( "host=" + hostaddr + " port=" + port + " user=" + user + " password=" + password + " dbname=" + dbname );
		// hostaddr=x.x.x.x only for IP-Addresses! Use host=xxx for a hostname.
	}

	ComponentOptions * DBLayer::get_options()
	{
		return new ComponentOptions();
	}

	void DBLayer::~DBLayer()
	{
		conn->disconnect();
	}

	/*
	*  Used to get a full node object out of the database. If no matching item is in the database no one will be created (use updateNode).
	*  Usefull for comparision to current states and updates.
	*  Also includes the nodes id, which is needed for further querying.
	*/
	Node * DBLayer::getNodeByHWID( string hwid )
	{
		// Create a new transaction. It gets automatically destroyed at the end of this funtion.
		work selectAction( *conn, "Select Transaction" );

		// Perform a select
		result resultSelect = selectAction.exec( ( "SELECT node_id FROM nodes WHERE hardware_uid='" + selectAction.esc( hwid ) + "'" ) );
		selectAction.commit();

		// Check if there is only one result
		if( resultSelect.size() == 1 ) {
			// Create Node and save the results
			unsigned int id = 0;
			Node * returns = new Node( hwid );

			// Check if results are sane (and convert them)
			if( !resultSelect[0]["node_id"].to( id ) ) {
				throw integretyError;
				return NULL;
			}
			returns->node_id = id;

			return returns;
		} else {
			// Not found
			return NULL;
		}
	}

	/*
	*  Used to update a node object in the database. If no corresponding node is found one will be created.
	*  The node is identified by node_id in the struct.
	*  Returns the nodes struct.
	*/
	void DBLayer::updateNode( Node * new_node )
	{
		if( DBLayer::getNodeByHWID( new_node->hw_id ) != NULL ) {
			// Entry already exists. Perform an update.

			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work updateAction( *conn, "Update Transaction" );

			// Perform the update
			updateAction.exec( ( "UPDATE nodes SET node_id=" + updateAction.esc( to_string( new_node->node_id ) ) + " WHERE hardware_uid='" + updateAction.esc( new_node->hw_id ) + "'" ) );
			updateAction.commit();
		} else {
			// Entry does not exists. Perform an insert.

			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work insertAction( *conn, "Insert Transaction" );

			// Perform the insert
			insertAction.exec( "INSERT INTO nodes (hardware_uid) VALUES ('" + insertAction.esc( new_node->hw_id ) + "')" );
			insertAction.commit();
			// Update the new_node reference to include the new node_id
			new_node = DBLayer::getNodeByHWID( new_node->hw_id );
		}

	}

	/*
	*  Returns a list of all storage devices which are connected to a node (specified by node_id).
	*  To get the node_id use getNodeByHWID.
	*/
	list<StorageDevice> * DBLayer::getAllStorageDevicesByNode( unsigned int node_id )
	{
		// Create a new transaction. It gets automatically destroyed at the end of this funtion.
		work selectAction( *conn, "Select Transaction" );

		// Perform a select. The result will be automatically destroyed.
		result resultSelect = selectAction.exec( "SELECT device_id, model_name, local_address FROM StorageDevices WHERE node_id=" + selectAction.esc( to_string( node_id ) ) + "" );
		selectAction.commit();

		// Check if there's at least one result
		if( resultSelect.size() > 0 ) {
			// Create Node and save the results
			unsigned int device_id;
			string model_name;
			string local_address;

			// Allocate space
			list<StorageDevice>* returns;
			returns = new list<StorageDevice>;
			// TODO: Will this be freed on the end of the function?
			StorageDevice * tempDevice;
			// Go thorugh all results
			for( result::size_type i = 0; i != resultSelect.size(); ++i ) {
				// Sanity checks and convertions
				if( !resultSelect[i]["device_id"].to( device_id ) ) {
					throw integretyError;
					return NULL;
				}
				if( !resultSelect[i]["model_name"].to( model_name ) ) {
					throw integretyError;
					return NULL;
				}
				if( !resultSelect[i]["local_address"].to( local_address ) ) {
					throw integretyError;
					return NULL;
				}
				// Create a new object, fill it and push it into the list
				tempDevice = new StorageDevice();
				tempDevice->device_id = device_id;
				tempDevice->model_name = model_name;
				tempDevice->local_address = local_address;
				tempDevice->node_id = node_id;
				returns->push_back( *tempDevice );

			}

			return returns;
		} else {
			// No results. Return NULL.
			return NULL;
		}
	}

	/*
	*  Returns a single storage device identified by node_id and device_id
	*  Get the node_id by using getNodeByHWID.
	*  Useful if the device_id is already known with anormaly incident.
	*/
	StorageDevice * DBLayer::getStorageDeviceByNodeAndDevice( unsigned int node_id, unsigned int device_id )
	{
		// Get all Storage Devices
		list<StorageDevice> * AllStorageDevices = DBLayer::getAllStorageDevicesByNode( node_id );
		// Run trough all storage devices
		if( AllStorageDevices != NULL ) {
			for( list<StorageDevice>::iterator it = AllStorageDevices->begin(); it != AllStorageDevices->end(); ++it ) {
				if( it->device_id == device_id ) return &*it;
			}
		}
		// Not found
		return NULL;
	}

	/* By Node und local_id */

	/*
	*  Used to update a StorageDevice.
	*  The device is identified by its node_id and device_id in the struct.
	*  If the corresponding device does not exist in the database it will be created.
	*/
	void DBLayer::updateStorageDevice( StorageDevice * new_StorageDevice )
	{
		if( DBLayer::getStorageDeviceByNodeAndDevice( new_StorageDevice->node_id, new_StorageDevice->device_id ) != NULL ) {
			// Entry already exists. Perform an update.
			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work updateAction( *conn, "Update Transaction" );

			// Perform the update
			updateAction.exec( "UPDATE StorageDevices SET model_name='" + updateAction.esc( to_string( new_StorageDevice->model_name ) ) + "', local_address='" + updateAction.esc( to_string( new_StorageDevice->local_address ) ) + "' WHERE node_id=" + updateAction.esc( to_string( new_StorageDevice->node_id ) ) + " AND device_id=" + updateAction.esc( to_string( new_StorageDevice->device_id ) ) + "" );
			updateAction.commit();
		} else {
			// Entry does not exists. Perform an insert.nschen, die auf Code starren. Es sind Frauen anwe
			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work insertAction( *conn, "Insert Transaction" );
			// Perform the insert
			insertAction.exec( "INSERT INTO StorageDevices (model_name, local_address, node_id) VALUES ('" + insertAction.esc( to_string( new_StorageDevice->model_name ) ) + "', '" + insertAction.esc( to_string( new_StorageDevice->local_address ) ) + "', '" + insertAction.esc( to_string( new_StorageDevice->node_id ) ) + "')" );
			insertAction.commit();

			// Update new_StorageDevice
			unsigned int id = 0;
			work selectAction( *conn, "Select Transaction" );
			result resultSelect = selectAction.exec( "SELECT device_id FROM StorageDevices WHERE model_name='" + insertAction.esc( to_string( new_StorageDevice->model_name ) ) + "' AND local_address = '" + insertAction.esc( to_string( new_StorageDevice->local_address ) ) + "' AND node_id = '" + insertAction.esc( to_string( new_StorageDevice->node_id ) ) + "'" );
			selectAction.commit();

			// Check if there is only one result
			if( resultSelect.size() == 1 ) {
				// Check if results are sane (and convert them)
				if( !resultSelect[0]["device_id"].to( id ) ) throw integretyError;
			} else {
				// Something went wrong
				//throw integretyError;
			}
			new_StorageDevice->device_id = id;
		}

	}

	/*
	*  Returns a Filesystem identified by its unique identifier.
	*  Filesystems are not node dependent.
	*/
	Filesystem * DBLayer::getFilesystemByUID( string uid )
	{
		// Create a new transaction. It gets automatically destroyed at the end of this funtion.
		work selectAction( *conn, "Select Transaction" );

		// Perform a select
		result resultSelect = selectAction.exec( "SELECT fs_id FROM Filesystems WHERE uid='" + selectAction.esc( uid ) + "'" );
		selectAction.commit();

		// Check if there is only one result
		if( resultSelect.size() == 1 ) {
			// Create Filesystem and save the results
			unsigned int id;
			Filesystem * returns = new Filesystem( uid );

			// Check if results are sane (and convert them)
			if( !resultSelect[0]["fs_id"].to( id ) ) {
				throw integretyError;
				return NULL;
			}
			returns->Filesystem_id = id;
			return returns;
		} else {
			// Something went wrong
			//throw integretyError;
			return NULL;
		}
	}

	/*
	*  Updates a Filesystem in the database. It is identified by its the unique identifier.
	*  If the corresponding Filesystem does not exist it will be created.
	*/
	void DBLayer::updateFilesystem( Filesystem * new_Filesystem )
	{
		if( DBLayer::getFilesystemByUID( new_Filesystem->unique_id ) != NULL ) {
			// Entry already exists. Perform an update.

			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work updateAction( *conn, "Update Transaction" );

			// Perform the update
			updateAction.exec( "UPDATE Filesystems SET fs_id=" + updateAction.esc( to_string( new_Filesystem->Filesystem_id ) ) + "WHERE uid='" + updateAction.esc( new_Filesystem->unique_id ) + "'" );
			updateAction.commit();
		} else {
			// Entry does not exists. Perform an insert.

			// Create a new transaction. It gets automatically destroyed at the end of this funtion.
			work insertAction( *conn, "Insert Transaction" );

			// Perform the insert
			insertAction.exec( "INSERT INTO Filesystems (uid) VALUES ('" + insertAction.esc( new_Filesystem->unique_id ) + "')" );
			insertAction.commit();

			// Update new_Filesystem reference to include the new fs_id
			new_Filesystem->Filesystem_id = DBLayer::getFilesystemByUID( new_Filesystem->unique_id )->Filesystem_id;
		}

	}
}


