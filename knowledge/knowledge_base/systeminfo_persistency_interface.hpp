#ifndef __DBLAYER_H
#define __DBLAYER_H

#include <iostream>
#include <list>
#include <string>
#include <pqxx/pqxx>
#include "systeminfo_containers.hpp"
using namespace std;
using namespace pqxx;

class DBLayer {
    private:
        connection* conn;
    public:
        /* Constructor. Get/Build up Connection */
        DBLayer();

        /* Destructor. Dismiss Connection, free remaining stuff */
        ~DBLayer();

        /*
        *  Used to get a full node object out of the database. If no matching item is in the database no one will be created (use updateNode).
        *  Usefull for comparision to current states and updates.
        *  Also includes the nodes id, which is needed for further querying.
        */
        SioxNode * getNodeByHWID(string hwid);

        /*
        *  Used to update a node object in the database. If no corresponding node is found one will be created.
        *  The node is identified by node_id in the struct.
        *  Returns the nodes struct.
        */
        // FIXME: How to handle empty struct fields?
        // FIXME: How to handle no node_id?
        void updateNode(SioxNode * new_node);

        /*
        *  Returns a list of all storage devices which are connected to a node (specified by node_id).
        *  To get the node_id use getNodeByHWID.
        */
        list<storage_device> * getAllStorageDevicesByNode(unsigned int node_id);

        /*
        *  Returns a single storage device identified by node_id and device_id
        *  Get the node_id by using getNodeByHWID.
        *  Useful if the device_id is already known with anormaly incident.
        */
        storage_device * getStorageDeviceByNodeAndDevice(unsigned int node_id, unsigned int device_id);

        /* By Node und local_id */

        /*
        *  Used to update a StorageDevice.
        *  The device is identified by its node_id and device_id in the struct.
        *  If the corresponding device does not exist in the database it will be created.
        */
        void updateStorageDevice(storage_device * new_storage_device);

        /*
        *  Returns a filesystem identified by its unique identifier.
        *  Filesystems are not node dependent.
        */
        filesystem * getFilesystemByUID(string uid);

        /*
        *  Updates a filesystem in the database. It is identified by its  filesystem id (not the unique identifier!).
        *  If the corresponding filesystem does not exist it will be created.
        */
        void updateFilesystem(filesystem * new_filesystem);

        /*
        *  To be extended in the future for more specific use cases, like plugins searching for nodes with similar hardware for comparision.
        */
};

#endif