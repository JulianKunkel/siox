#ifndef __SYSTEMINFO_CONTAINERS
#define __SYSTEMINFO_CONTAINERS

#include <string>
using namespace std;

class SioxNode {
    public:
        SioxNode(unsigned int hw_id);
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      node_id;
        /* The hardware id. May only be 255 characters long for now (though the database could do more) */
        string       hw_id;
        /*
        * Other metrics here
        */

};

class storage_device {
    public:
        // FIXME: Constructor?
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      device_id;
        /* The node id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int      node_id;
        /* The model name.  May only be 255 characters long for now (though the database could do more) */
        char        model_name[255];
        /* The local adress of the device.  May only be 255 characters long for now (though the database could do more) */
        char        local_address[255];
        /*
        * Other metrics here
        */
};

class filesystem {
    public:
        // FIXME: Constructor
        /* The filesystem id. Serial (database type) saves up to 2^16, so int16 is fine */
        unsigned int       filesystem_id;
        /* The unique identifier of the filesystem.  May only be 255 characters long for now (though the database could do more) */
        char        unique_id[255];
        /*
        * Other metrics here
        */
};

#endif