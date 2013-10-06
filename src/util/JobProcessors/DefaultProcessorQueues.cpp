#include <assert.h>

#include <util/DefaultProcessorQueues.hpp>

namespace util{

void FIFOProcessorQueue::enqueueJob( void * job ){
	pendingJobs.push_back(job);
}

bool FIFOProcessorQueue::empty(){
	return pendingJobs.empty();
}

void * FIFOProcessorQueue::pollNextJob(){
	assert(! pendingJobs.empty());
	void * job =  pendingJobs.front();
	pendingJobs.pop_front();
	return job;
}

bool FIFOProcessorQueue::removeJob(void * job){
	unsigned int count = pendingJobs.size();
	pendingJobs.remove(job);
	return count != pendingJobs.size();
}

void FIFOProcessorQueue::enqueueFront( void * job ){
	pendingJobs.push_front(job);
}

ProcessorQueueDiscardWrapper::ProcessorQueueDiscardWrapper(int maxPendingJobs, ProcessorQueue * queue) : maxPendingJobs(maxPendingJobs), currentJobs(currentJobs), queue(queue) {}

void ProcessorQueueDiscardWrapper::enqueueJob( void * job ){
	currentJobs++;
	queue->enqueueJob(job);
}

bool ProcessorQueueDiscardWrapper::mayEnqueue(){
	return currentJobs < maxPendingJobs;
}	

bool ProcessorQueueDiscardWrapper::empty(){
	return currentJobs == 0;
}

void * ProcessorQueueDiscardWrapper::pollNextJob(){
	assert(currentJobs > 0);
	currentJobs--;
	return queue->pollNextJob();
}

bool ProcessorQueueDiscardWrapper::removeJob(void * job){
	bool ret = queue->removeJob(job);
	if ( ret ){
		currentJobs--;
	}
	return ret;
}

void ProcessorQueueDiscardWrapper::enqueueFront( void * job ){
	// we must obey and enqueue the operation

	currentJobs++;
	queue->enqueueFront(job);	
}


}