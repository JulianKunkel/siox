#include <assert.h>
#include <iostream>

#include <core/module/module-loader.hpp>

#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

#include "../FileBasedSystemInformationOptions.hpp"

using namespace std;

using namespace monitoring;
using namespace core;

int main(int argc, char const *argv[]){
	SystemInformationGlobalIDManager * s = core::module_create_instance<SystemInformationGlobalIDManager>("", "FileBasedSystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE);

	// init plugin
	// not necessary, but for testing...
	FileBasedSystemInformationOptions * op = new FileBasedSystemInformationOptions();
	op->filename = "system-info-example.txt";
	s->init(op);


	UniqueInterfaceID uid = s->interface_id("MPI", "MPICH2");
	assert(is_valid_id(uid));
	string inref = s->interface_name(uid);
	assert(inref == "MPI");
	string interface_implementation = s->interface_implementation(uid);
	assert(interface_implementation == "MPICH2");

	UniqueInterfaceID uid2 = s->interface_id("MPI", "OpenMPI");
	assert(is_valid_id(uid2));
	inref = s->interface_name(uid2);
	assert(inref == "MPI");
	interface_implementation = s->interface_implementation(uid2);
	assert(interface_implementation == "OpenMPI");

	uid2 = s->interface_id("MPI", "MPICH2");

	assert(uid2.interface == uid.interface);
	assert(uid2.implementation == uid.implementation);



	NodeID nid = s->node_id("testhost");
	assert(s->node_id("testhost") == nid);
	assert(s->node_hostname(nid) == "testhost");

	DeviceID did = s->device_id(nid, "/dev/ssd1");
	assert(s->device_id(nid, "/dev/ssd1") == did);
	assert(s->node_of_device(did) == nid);
	assert(s->device_local_name(did) == "/dev/ssd1");


	FilesystemID fid = s->filesystem_id("pvfs2://10.0.0.1");
	assert( fid == s->filesystem_id("pvfs2://10.0.0.1"));
	assert(s->filesystem_name(fid) == "pvfs2://10.0.0.1");


	UniqueComponentActivityID aid = s->activity_id(uid, "open");
	assert(aid == s->activity_id(uid, "open"));
	assert(aid != s->activity_id(uid, "open2"));

	assert(s->interface_of_activity(aid) == uid);
	assert(s->activity_name(aid) == "open");

	delete(s);

	cout << "OK" << endl;

	return 0;
}


