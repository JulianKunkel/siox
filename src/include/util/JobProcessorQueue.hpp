#ifndef CORE_PROCESSOR_QUEUE_HPP
#define CORE_PROCESSOR_QUEUE_HPP

namespace util{
	/*
 		The ProcessorQueue offers an abstraction to manage pending work of any kind.
 		It is NOT thread safe. Thread safety must be enforced by the JobPocessor.
 	*/
	class ProcessorQueue {
	public:
		virtual bool mayEnqueue(){
			return true;
		}

		virtual void enqueueFront( void * job ) = 0;

		virtual void enqueueJob( void * job ) = 0;

		virtual bool empty() = 0;

		virtual void * pollNextJob() = 0;

		// Remove a specific job from the queue
		// Returns true if the job could be removed
		virtual bool removeJob(void * job) = 0;

		virtual ~ProcessorQueue(){};
	};
}

#endif