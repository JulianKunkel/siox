#include <stdio.h>
#include <stdlib.h>
#include "systeminfo_containers.hpp"
#include "systeminfo_persistency_interface.hpp"
// No conflicts between std and pq
using namespace std;
using namespace pqxx;

/*
 WIP WIP WIP WIP WIP
 Not workin yet.
*/

/* Constructor. Get/Build up Connection */
DBLayer::DBLayer() {
    // host=test is a hostname! Use hostaddr=x.x.x for an IP-Adress.
    // TODO: Catch exception
    conn = new connection("host=test port=1337 user=test password=test dbname=test");
}

/* Destructor. Dismiss Connection, free remaining stuff */
DBLayer::~DBLayer() {

}

/*
*  Used to get a full node object out of the database. If no matching item is in the database no one will be created (use updateNode).
*  Usefull for comparision to current states and updates.
*  Also includes the nodes id, which is needed for further querying.
*/
SioxNode * DBLayer::getNodeByHWID(string hwid) {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    // TODO: Catch exception
    result resultSelect = selectAction.exec("SELECT node_id FROM nodes WHERE hardware_uid=%s", hwid);

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Create SioxNode and save the results
        unsigned int id;
        SioxNode* returns = new SioxNode(hwid);

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["node_id"].to(id)) return NULL;
            returns->node_id = id;
            return returns;
    }
    else {
        // Something went wrong
        // TODO: What to do when resultSelect.size() > 1?
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
        // TODO: Catch exception
        updateAction.exec(("UPDATE nodes SET node_id=%s WHERE hardware_uid=%s", to_string(new_node->node_id), new_node->hw_id));
    }
    else {
        // Entry does not exists. Perform an insert.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");

        // Perform the insert
        // TODO: Catch exception
        insertAction.exec("INSERT INTO nodes (hw_id) VALUES (%s)", new_node->hw_id);

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
    // TODO: Catch exception
    result resultSelect = selectAction.exec("SELECT device_id, model_name, local_address FROM storage_devices WHERE node_id=%s", to_string(node_id));

    // Check if there's at least one result
    if (resultSelect.size() > 0) {
        // Create SioxNode and save the results
        unsigned int device_id;
        string model_name;
        string local_address;

        // Allocate space
        list<storage_device>* returns;
        // TODO: Will this be freed on the end of the function?
        storage_device* tempDevice;

        // Go thorugh all results
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            // Sanity checks and convertions
            if(!resultSelect[i]["device_id"].to(device_id)) return NULL;
            if(!resultSelect[i]["model_name"].to(model_name)) return NULL;
            if(!resultSelect[i]["local_address"].to(local_address)) return NULL;

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
    for (list<storage_device>::iterator it=AllStorageDevices->begin(); it != AllStorageDevices->end(); ++it) {
        if (it->device_id == device_id) return &*it;
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
        // TODO: Catch exception
        updateAction.exec(("UPDATE storage_devices SET model_name=%s, local_address=%s WHERE node_id=%s AND device_id=%s", to_string(new_storage_device->model_name), to_string(new_storage_device->local_address), to_string(new_storage_device->node_id), to_string(new_storage_device->device_id)));
    }
    else {
        // Entry does not exists. Perform an insert.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");

        // Perform the insert
        // TODO: Catch exception
        // TODO: Really insert all entries?
        insertAction.exec(("INSERT INTO storage_devices (model_name, local_address, node_id, device_id) VALUES (%s, %s, %s, %s)", to_string(new_storage_device->model_name), to_string(new_storage_device->local_address), to_string(new_storage_device->node_id), to_string(new_storage_device->device_id)));

        // Update new_storage_device
        new_storage_device = DBLayer::getStorageDeviceByNodeAndDevice(new_storage_device->node_id, new_storage_device->device_id);
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
    // TODO: Catch exception
    result resultSelect = selectAction.exec("SELECT fs_id FROM filesystems WHERE uid=%s", uid);

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Create filesystem and save the results
        unsigned int id;
        filesystem* returns = new filesystem();

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["fs_id"].to(id)) return NULL;
            returns->filesystem_id = id;
            return returns;
    }
    else {
        // Something went wrong
        // TODO: What to do when resultSelect.size() > 1?
        return NULL;
    }
}

/*
*  Updates a filesystem in the database. It is identified by its  filesystem id (not the unique identifier!).
*  If the corresponding filesystem does not exist it will be created.
*/
void DBLayer::updateFilesystem(filesystem * new_filesystem) {
    if(DBLayer::getFilesystemByUID(new_filesystem->unique_id) != NULL) {
        // Entry already exists. Perform an update.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work updateAction(*conn, "Update Transaction");

        // Perform the update
        // TODO: Catch exception
        updateAction.exec(("UPDATE filesystems SET fs_id=%s WHERE uid=%s", to_string(new_filesystem->filesystem_id), new_filesystem->unique_id));
    }
    else {
        // Entry does not exists. Perform an insert.

        // Create a new transaction. It gets automatically destroyed at the end of this funtion.
        work insertAction(*conn, "Insert Transaction");

        // Perform the insert
        // TODO: Catch exception
        insertAction.exec("INSERT INTO filesystems (unid) VALUES (%s)", new_filesystem->unique_id);

        // Update new_filesystem reference to include the new fs_id
        new_filesystem = DBLayer::getFilesystemByUID(new_filesystem->unique_id);
    }

}

/*
*  This is here for testing purposes. Will be deleted.
*/
int main( int argc, const char* argv[] )
{
    DBLayer* test = new DBLayer();
}