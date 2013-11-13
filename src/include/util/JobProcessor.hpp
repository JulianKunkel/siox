#ifndef CORE_PROCESSOR_HPP
#define CORE_PROCESSOR_HPP

#include "JobProcessorQueue.hpp"

namespace util{
	class ProcessorCallback{
	protected:
		virtual void processJob(void * job) = 0;
		virtual void jobAborted(void * job) = 0;
	};


	class JobProcessor : public ProcessorCallback {
	protected:
		ProcessorQueue * queue = nullptr;
	public:
		void setProcessorQueue(ProcessorQueue * queue){
			this->queue = queue;
		}

		inline bool mayStartJob(){
			return queue->mayEnqueue();
		}

		virtual void stopProcessing() = 0;

		// sets enabledProcessing = true and restart internal processing.
		virtual void startProcessing() = 0;

		/* Start a job asynchronously */
		virtual void iStartJob(void * job) = 0;

		/* Stop a job, if possible -- best-effort */
		virtual void iCancelJob(void * job) = 0;

		/* Try to complete all pending jobs or abort them if terminate = true */
		virtual void shutdown(bool terminate = false) = 0;

		/* Should destroy all pending jobs immediately */
		virtual ~JobProcessor(){ 
			delete(queue);
		}
	protected:
		// helper function which cleans all pending jobs
		void abortPendingJobs(){
			while( ! queue->empty() ){
				void * job = queue->pollNextJob();
				jobAborted(job);
			}
		}
	};
}

#endif