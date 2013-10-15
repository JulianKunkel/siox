#ifndef UTIL_JOB_PROCESSORS_DEFAULTQUEUE_HPP
#define UTIL_JOB_PROCESSORS_DEFAULTQUEUE_HPP

#include <list>

#include <util/JobProcessorQueue.hpp>

namespace util{

class FIFOProcessorQueue : public ProcessorQueue{
private:
	std::list<void*> pendingJobs;
public:
	virtual void enqueueJob( void * job );

	virtual bool empty();

	virtual void * pollNextJob();

	virtual bool removeJob(void * job);

	virtual void enqueueFront( void * job );
};

class ProcessorQueueDiscardWrapper : public ProcessorQueue{
private:
	int maxPendingJobs;
	int currentJobs;
	ProcessorQueue * queue;
public:

	ProcessorQueueDiscardWrapper(int maxPendingJobs, ProcessorQueue * queue);

	virtual void enqueueJob( void * job );

	virtual bool mayEnqueue();

	virtual bool empty();

	virtual void * pollNextJob();

	virtual bool removeJob(void * job);

	virtual void enqueueFront( void * job );

	~ProcessorQueueDiscardWrapper();
};

}

#endif