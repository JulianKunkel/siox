#ifndef __SYSTEMINFO_CONTAINERS
#define __SYSTEMINFO_CONTAINERS

#include <string>
using namespace std;

class SioxNode {
    public:
        SioxNode(string p_hw_id);
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      node_id;
        /* The hardware id. */
        string       hw_id;
        /*
        * Other metrics here
        */

};

class storage_device {
    public:
        storage_device();
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      device_id;
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      node_id;
        /* The model name. */
        string        model_name;
        /* The local adress of the device. */
        string        local_address;
        /*
        * Other metrics here
        */
};

class filesystem {
    public:
        // FIXME: Constructor
        /* The filesystem id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int       filesystem_id;
        /* The unique identifier of the filesystem. */
        string        unique_id;
        /*
        * Other metrics here
        */
};

#endif