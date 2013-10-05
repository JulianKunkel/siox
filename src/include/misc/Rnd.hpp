#ifndef SIOX_RANDOM_H
#define SIOX_RANDOM_H

#include <monitoring/datatypes/ids.hpp>
#include <monitoring/datatypes/Activity.hpp>

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
	
};

#endif

