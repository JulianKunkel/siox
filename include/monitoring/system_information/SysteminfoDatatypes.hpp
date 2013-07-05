#ifndef __SYSTEMINFO_CONTAINERS
#define __SYSTEMINFO_CONTAINERS

#include <string>

#include <monitoring/datatypes/ids.hpp>


using namespace std;

namespace monitoring{

namespace system_information{

class Node {
    public:
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        NodeID        node_id;
        /* The hardware id. */
        string       hw_id;
        /*
        * Other metrics here
        */
        bool equals(Node* node);

};

class StorageDevice {
    public:
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        DeviceID      device_id;
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        NodeID          node_id;
        /* The model name. */
        string        model_name;
        /* The local adress of the device. */
        string        local_address;
        /*
        * Other metrics here
        */
        bool equals(StorageDevice* device);
};

class Filesystem {
    public:
        /* The filesystem id. Serial (database type) saves up to 2^16, so int16 is fine */
        FilesystemID       filesystem_id;
        /* The unique identifier of the filesystem. */
        string        unique_id;
        /*
        * Other metrics here
        */
        bool equals(Filesystem* fs);
};

}

}

#endif
