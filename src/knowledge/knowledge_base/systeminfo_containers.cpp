#include <string>

#include <knowledge/knowledge_base/systeminfo_containers.hpp>

using namespace std;

namespace knowledge {

	Node::Node( string p_hw_id )
	{
		hw_id = p_hw_id;
		node_id = 0;
	}

	bool Node::equals( Node * node )
	{
		return ( ( node->node_id == this->node_id ) && ( node->hw_id == this->hw_id ) );
	}

	StorageDevice::StorageDevice()
	{
		device_id = 0;
		node_id = 0;
		model_name = "";
		local_address = "";
	}

	bool StorageDevice::equals( StorageDevice * device )
	{
		return ( ( device->device_id == this->device_id ) && ( device->node_id == this->node_id ) && ( device->model_name == this->model_name ) && ( device->local_address == this->local_address ) );
	}

	Filesystem::Filesystem( string uid )
	{
		unique_id = uid;
		filesystem_id = 0;
	}

	bool Filesystem::equals( Filesystem * fs )
	{
		return ( ( fs->filesystem_id == this->filesystem_id ) && ( fs->unique_id == this->unique_id ) );
	}

}
