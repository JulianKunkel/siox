#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "systeminfo_containers.hpp"
#include "systeminfo_persistency_interface.hpp"

// No conflicts between std and pqxx
using namespace std;
using namespace pqxx;

/*
 On SQL-Injections

 pqxx's work-object has a method called "esc", which takes and returns a string.
 Use it on _any_ user-changeable string before pushing it into the database.
*/

// Exception for things that shouldn't go wrong (and thus arn't addresses by the databases exception handling)
class integrityException: public exception
{
  virtual const char* what() const throw()
  {
    return "Database returned sometihing unexpected.";
  }
} integretyError;

/* Constructor. Get/Build up Connection */
DBLayer::DBLayer() {
    // hostaddr=x.x.x.x only for IP-Addresses! Use host=xxx for a hostname.
    conn = new connection("hostaddr=136.172.14.14 port=48142 user=postgres password=postgres dbname=systeminfo_test");
}

/* Destructor. Dismiss Connection, free remaining stuff */
DBLayer::~DBLayer() {
    conn->disconnect();
}

/*
*  Used to get a full node object out of the database. If no matching item is in the database no one will be created (use updateNode).
*  Usefull for comparision to current states and updates.
*  Also includes the nodes id, which is needed for further querying.
*
*/
SioxNode * DBLayer::getNodeByHWID(string hwid) {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT node_id FROM nodes WHERE hardware_uid='" + selectAction.esc(hwid) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Create SioxNode and save the results
        unsigned int id = 0;
        SioxNode* returns = new SioxNode(hwid);

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["node_id"].to(id)) {
            throw integretyError;
            return NULL;
        }
        returns->node_id = id;

        return returns;
    }
    else {
        // Not found
        return NULL;
    }
}

/*
*  Used to update a node object in the database. If no corresponding node is found one will be created.
*  The node is identified by node_id in the struct.
*  Returns the nodes struct.
*/
void DBLayer::updateNode(SioxNode * new_node) {
    if(DBLayer::getNodeByHWID(new_node->hw_id) != NULL) {
        // Entry already exists. Perform an update.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work updateAction(*conn, "Update Transaction");

        // Perform the update
        updateAction.exec(("UPDATE nodes SET node_id="+updateAction.esc(to_string(new_node->node_id))+" WHERE hardware_uid='"+updateAction.esc(new_node->hw_id)+"'"));
        updateAction.commit();
    }
    else {
        // Entry does not exists. Perform an insert.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");

        // Perform the insert
        insertAction.exec("INSERT INTO nodes (hardware_uid) VALUES ('"+insertAction.esc(new_node->hw_id)+"')");
        insertAction.commit();
        // Update the new_node reference to include the new node_id
        new_node = DBLayer::getNodeByHWID(new_node->hw_id);
    }

}

/*
*  Returns a list of all storage devices which are connected to a node (specified by node_id).
*  To get the node_id use getNodeByHWID.
*/
list<storage_device> * DBLayer::getAllStorageDevicesByNode(unsigned int node_id) {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select. The result will be automatically destroyed.
    result resultSelect = selectAction.exec("SELECT device_id, model_name, local_address FROM storage_devices WHERE node_id="+selectAction.esc(to_string(node_id))+"");
    selectAction.commit();

    // Check if there's at least one result
    if (resultSelect.size() > 0) {
        // Create SioxNode and save the results
        unsigned int device_id;
        string model_name;
        string local_address;

        // Allocate space
        list<storage_device>* returns;
        returns = new list<storage_device>;
        // TODO: Will this be freed on the end of the function?
        storage_device* tempDevice;
        // Go thorugh all results
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            // Sanity checks and convertions
            if(!resultSelect[i]["device_id"].to(device_id)) {
                throw integretyError;
                return NULL;
            }
            if(!resultSelect[i]["model_name"].to(model_name)) {
                throw integretyError;
                return NULL;
            }
            if(!resultSelect[i]["local_address"].to(local_address)) {
                throw integretyError;
                return NULL;
            }
            // Create a new object, fill it and push it into the list
            tempDevice = new storage_device();
            tempDevice->device_id = device_id;
            tempDevice->model_name = model_name;
            tempDevice->local_address = local_address;
            tempDevice->node_id = node_id;
            returns->push_back(*tempDevice);

        }

        return returns;
    }
    else {
        // No results. Return NULL.
        return NULL;
    }
}

/*
*  Returns a single storage device identified by node_id and device_id
*  Get the node_id by using getNodeByHWID.
*  Useful if the device_id is already known with anormaly incident.
*/
storage_device * DBLayer::getStorageDeviceByNodeAndDevice(unsigned int node_id, unsigned int device_id) {
    // Get all Storage Devices
    list<storage_device> * AllStorageDevices = DBLayer::getAllStorageDevicesByNode(node_id);
    // Run trough all storage devices
    if(AllStorageDevices != NULL) {
        for (list<storage_device>::iterator it=AllStorageDevices->begin(); it != AllStorageDevices->end(); ++it) {
            if (it->device_id == device_id) return &*it;
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
void DBLayer::updateStorageDevice(storage_device * new_storage_device) {
    if(DBLayer::getStorageDeviceByNodeAndDevice(new_storage_device->node_id, new_storage_device->device_id) != NULL) {
        // Entry already exists. Perform an update.
        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work updateAction(*conn, "Update Transaction");

        // Perform the update
        updateAction.exec("UPDATE storage_devices SET model_name='"+updateAction.esc(to_string(new_storage_device->model_name))+"', local_address='"+updateAction.esc(to_string(new_storage_device->local_address))+"' WHERE node_id="+updateAction.esc(to_string(new_storage_device->node_id))+" AND device_id="+updateAction.esc(to_string(new_storage_device->device_id))+"");
        updateAction.commit();
    }
    else {
        // Entry does not exists. Perform an insert.nschen, die auf Code starren. Es sind Frauen anwe
        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");
        // Perform the insert
        insertAction.exec("INSERT INTO storage_devices (model_name, local_address, node_id) VALUES ('"+insertAction.esc(to_string(new_storage_device->model_name))+"', '"+insertAction.esc(to_string(new_storage_device->local_address))+"', '"+insertAction.esc(to_string(new_storage_device->node_id))+"')");
        insertAction.commit();

        // Update new_storage_device
        unsigned int id = 0;
        work selectAction(*conn, "Select Transaction");
        result resultSelect = selectAction.exec("SELECT device_id FROM storage_devices WHERE model_name='"+insertAction.esc(to_string(new_storage_device->model_name))+"' AND local_address = '"+insertAction.esc(to_string(new_storage_device->local_address))+"' AND node_id = '"+insertAction.esc(to_string(new_storage_device->node_id))+"'");
        selectAction.commit();

        // Check if there is only one result
        if (resultSelect.size() == 1) {
            // Check if results are sane (and convert them)
            if (!resultSelect[0]["device_id"].to(id)) throw integretyError;
        }
        else {
            // Something went wrong
            //throw integretyError;
        }
        new_storage_device->device_id = id;
    }

}

/*
*  Returns a filesystem identified by its unique identifier.
*  Filesystems are not node dependent.
*/
filesystem * DBLayer::getFilesystemByUID(string uid) {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec("SELECT fs_id FROM filesystems WHERE uid='"+selectAction.esc(uid)+"'");
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Create filesystem and save the results
        unsigned int id;
        filesystem* returns = new filesystem(uid);

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["fs_id"].to(id)) {
            throw integretyError;
            return NULL;
        }
        returns->filesystem_id = id;
        return returns;
    }
    else {
        // Something went wrong
        //throw integretyError;
        return NULL;
    }
}

/*
*  Updates a filesystem in the database. It is identified by its the unique identifier.
*  If the corresponding filesystem does not exist it will be created.
*/
void DBLayer::updateFilesystem(filesystem * new_filesystem) {
    if(DBLayer::getFilesystemByUID(new_filesystem->unique_id) != NULL) {
        // Entry already exists. Perform an update.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work updateAction(*conn, "Update Transaction");

        // Perform the update
        updateAction.exec("UPDATE filesystems SET fs_id="+updateAction.esc(to_string(new_filesystem->filesystem_id))+"WHERE uid='"+updateAction.esc(new_filesystem->unique_id)+"'");
        updateAction.commit();
    }
    else {
        // Entry does not exists. Perform an insert.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");

        // Perform the insert
        insertAction.exec("INSERT INTO filesystems (uid) VALUES ('"+insertAction.esc(new_filesystem->unique_id)+"')");
        insertAction.commit();

        // Update new_filesystem reference to include the new fs_id
        new_filesystem->filesystem_id = DBLayer::getFilesystemByUID(new_filesystem->unique_id)->filesystem_id;
    }

}