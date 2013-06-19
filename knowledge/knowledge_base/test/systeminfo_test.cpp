#include "../systeminfo_persistency_interface.hpp"
#include <assert.h>

int main( int argc, const char* argv[] )
{
    DBLayer* test = new DBLayer();

    // Node stuff test
    assert(test->getNodeByHWID("test") == NULL);

    SioxNode* testNode = new SioxNode("test");

    test->updateNode(testNode);

    assert(test->getNodeByHWID("test") != NULL);
    assert(test->getNodeByHWID("test")->node_id > 0);
    assert(test->getNodeByHWID("test")->hw_id == testNode->hw_id);
    testNode->node_id = testNode->node_id+1;
    test->updateNode(testNode);

    assert(test->getNodeByHWID("test") != NULL);
    assert(test->getNodeByHWID("test")->node_id == testNode->node_id);
    assert(test->getNodeByHWID("test")->hw_id == testNode->hw_id);

    // Storage Devices test
    assert(test->getAllStorageDevicesByNode(testNode->node_id) == NULL);

    storage_device* testDevice = new storage_device();
    testDevice->node_id = testNode->node_id;
    testDevice->model_name = "model_test";
    testDevice->local_address = "address_test";

    test->updateStorageDevice(testDevice);

    assert(testDevice->device_id > 0);
    assert(test->getAllStorageDevicesByNode(testNode->node_id) != NULL);
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->size() == 1);

    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->model_name == "model_test");
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->local_address == "address_test");
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->node_id == testNode->node_id);

    assert(test->getStorageDeviceByNodeAndDevice(testNode->node_id, testDevice->device_id)->equals(testDevice));

    testDevice->model_name = "model_test_new";
    testDevice->local_address = "address_test_new";

    test->updateStorageDevice(testDevice);

    assert(testDevice->device_id > 0);
    assert(test->getAllStorageDevicesByNode(testNode->node_id) != NULL);
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->size() == 1);

    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->model_name == "model_test_new");
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->local_address == "address_test_new");
    assert(test->getAllStorageDevicesByNode(testNode->node_id)->begin()->node_id == testNode->node_id);

    // filesystems test
    assert(test->getFilesystemByUID("test") == NULL);

    filesystem* fs = new filesystem("test");

    test->updateFilesystem(fs);

    assert(test->getFilesystemByUID("test") != NULL);
    assert(test->getFilesystemByUID("test")->filesystem_id > 0);
    assert(test->getFilesystemByUID("test")->unique_id == "test");

    fs->filesystem_id = fs->filesystem_id+1;

    test->updateFilesystem(fs);

    assert(test->getFilesystemByUID("test") != NULL);
    assert(test->getFilesystemByUID("test")->filesystem_id == fs->filesystem_id);
    assert(test->getFilesystemByUID("test")->unique_id == "test");

    // Delete all StorageDevice stuff
    connection* conn = new connection("hostaddr=136.172.14.14 port=48142 user=postgres password=postgres dbname=systeminfo_test");
    work deleteAction2(*conn, "Delete Transaction 2");
    deleteAction2.exec("DELETE FROM storage_devices WHERE node_id ="+to_string(testNode->node_id));
    deleteAction2.commit();

    // Delete all filesystem stuff
    work deleteAction3(*conn, "Delete Transaction 3");
    deleteAction3.exec("DELETE FROM filesystems WHERE uid='test'");
    deleteAction3.commit();

    // Delete all node stuff
    work deleteAction(*conn, "Delete Transaction");
    deleteAction.exec("DELETE FROM nodes WHERE node_id ="+to_string(testNode->node_id));
    deleteAction.commit();

    printf("All tests passed.\n");
}