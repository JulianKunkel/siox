#include <string>
#include "systeminfo_containers.hpp"
using namespace std;


SioxNode::SioxNode(string p_hw_id) {
    hw_id = p_hw_id;
    node_id = 0;
}

storage_device::storage_device() {
    device_id = 0;
    node_id = 0;
    model_name = "";
    local_address = "";
}