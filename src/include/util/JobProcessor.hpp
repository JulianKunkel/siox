#ifndef CORE_PROCESSOR_HPP
#define CORE_PROCESSOR_HPP

#include "JobProcessorQueue.hpp"

namespace util{
	class ProcessorCallback{
	protected:
		virtual void processJob(void * job) = 0;
		virtual void jobAborted(void * job){};
	};


	class JobProcessor : public ProcessorCallback {
	protected:
		ProcessorQueue * queue = nullptr;
		bool enabledProcessing = true;
	public:
		void setProcessorQueue(ProcessorQueue * queue){
			this->queue = queue;
		}

		inline bool mayStartJob(){
			return queue->mayEnqueue();
		}

		inline void stopProcessing(){
			enabledProcessing = false;
		}

		// sets enabledProcessing = true and restart internal processing.
		virtual void startProcessing() = 0;

		/* Start a job asynchronously */
		virtual void iStartJob(void * job) = 0;

		/* Stop a job, if possible -- best-effort */
		virtual void iCancelJob(void * job) = 0;

		/* Try to complete all pending jobs */
		virtual void shutdown() = 0;

		/* Destroys all pending jobs immediately */
		virtual void terminate() = 0;

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