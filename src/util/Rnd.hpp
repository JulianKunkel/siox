#ifndef SIOX_RANDOM_H
#define SIOX_RANDOM_H

#include <algorithm>

#include <monitoring/datatypes/ids.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/system_information/SysteminfoDatatypes.hpp>

using namespace monitoring;
using namespace system_information;

namespace rnd {

	inline unsigned int randr(unsigned int min = 1, unsigned int max = 1000)
	{
		double scaled = (double) rand() / RAND_MAX;
		return (max - min +1)*scaled + min;
	}	

	inline NodeID node_id()
	{
		return (NodeID) randr();
	}
	
	
	inline ProcessID *process_id()
	{
		ProcessID *pid = new ProcessID;
		pid->nid  = node_id();
		pid->pid  = randr();
		pid->time = randr();
		
		return pid;
	}
	
	
	inline ComponentID *component_id()
	{
		ComponentID *cid = new ComponentID;
		cid->pid = *process_id();
		cid->id = randr();
		
		return cid;
	}
	
	
	inline ActivityID *activity_id()
	{
		ActivityID *aid = new ActivityID;
		
		aid->id     = randr();
		aid->thread = randr();
		aid->cid    = *component_id();
		
		return aid;
	}
	
	
	inline RemoteCallIdentifier *remote_call_id()
	{
		RemoteCallIdentifier *rcid = new RemoteCallIdentifier(node_id(), randr(), randr());
		
		return rcid;
	}
	
	
	inline std::string randr_string(size_t length)
	{
		auto randchar = []() -> char {
			const char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};
		
		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);
		return str;
	}
	
	
	inline AttributeValue *attribute_value()
	{
		AttributeValue *av;
		
		switch(randr(0, 7)) {
		case 0:
			av = new AttributeValue((int32_t) randr());
			break;
		case 1:
			av = new AttributeValue((uint32_t) randr());
			break;
		case 2:
			av = new AttributeValue((int64_t) randr());
			break;
		case 3:
			av = new AttributeValue((uint64_t) randr());
			break;
		case 4:
			av = new AttributeValue((float) randr());
			break;
		case 5:
			av = new AttributeValue((double) randr());
			break;
		case 6:
			av = new AttributeValue(randr_string(20).c_str());
			break;
		case 7:
		default:
			av = new AttributeValue();
		}
		
		return av;
	}
	
	
	inline Attribute *attribute()
	{
		Attribute *attr = new Attribute(randr(), *attribute_value());
		
		return attr;
	}
	
	
	inline RemoteCall *remote_call()
	{
		RemoteCall *rc = new RemoteCall;
		
		vector<Attribute> *attributes = new vector<Attribute>;
		
		int num_attr = randr(0, 10);
		for (int i = 0; i < num_attr; i++) 
			attributes->push_back(*attribute());
		
		rc->target = *remote_call_id();
		rc->attributes = *attributes;
		
		return rc;
	}
	
	
	inline Activity *activity()
	{
		ActivityID *aid = activity_id();
		
		UniqueComponentActivityID ucaid = randr();
		
		Timestamp time_start = randr();
		Timestamp  time_stop = time_start + randr();
		
		RemoteCallIdentifier *invoker = remote_call_id();
		
		ActivityError error_value = randr();
		
		vector<ActivityID> parents;
		int num_parents = randr(0, 20);
		for (int i = 0; i < num_parents; i++) 
			parents.push_back(*activity_id());
		
		
		vector<Attribute> attributes;
		int num_attrs = randr(0, 20);
		for (int i = 0; i < num_attrs; i++)
			attributes.push_back(*attribute());
		
		vector<RemoteCall> remote_calls;
		int num_rcs = randr(0, 20);
		for (int i = 0; i < num_rcs; i++)
			remote_calls.push_back(*remote_call());
		
		Activity *act = new Activity(ucaid, time_start, time_stop, *aid, parents, attributes, remote_calls, invoker, error_value);
		
		return act;
	}
	
	
	inline Node *node()
	{
		Node *n = new Node();
		n->node_id = node_id();
		n->hw_id = randr_string(10);
		
		return n;
	}
	
	
	inline DeviceID device_id()
	{
		return (DeviceID) randr();
	}
	
	
	inline StorageDevice *storage_device()
	{
		StorageDevice *sd = new StorageDevice();
		
		sd->device_id = device_id();
		sd->node_id = node_id();
		sd->model_name = randr_string(10);
		sd->local_address = randr_string(10);
		return sd;
	}
	
	
	inline FilesystemID file_system_id()
	{
		return (FilesystemID) randr();
	}
	
	
	inline Filesystem *file_system()
	{
		Filesystem *fs = new Filesystem();
		fs->filesystem_id = file_system_id();
		fs->unique_id = randr_string(10);
		
		return fs;
	}
	
};

#endif

