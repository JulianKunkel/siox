#include <assert.h>

#include <util/threadSafety.h>
#include "SingleThreadedJobProcessor.hpp"

using namespace std;

namespace util{


void SingleThreadedJobProcessor::iStartJob(void * job){
	unique_lock<mutex> lk(m);
	
	if( queue->mayEnqueue() ){
		queue->enqueueJob(job);
		// wakeup a pending threads
		if( status == OperationalStatus::OPERATIONAL ){
			cv.notify_one();
		}
	}else{
		jobAborted(job);
	}
}

void SingleThreadedJobProcessor::iCancelJob(void * job){
	unique_lock<mutex> lk(m);
	queue->removeJob(job);
}


void SingleThreadedJobProcessor::startProcessing(){
	unique_lock<mutex> lk(m);
	if ( myThread != nullptr ){
		return;
	}
	cv.notify_one();
	myThread = new thread( & SingleThreadedJobProcessor::process, this );
}

void SingleThreadedJobProcessor::stopProcessing(){
	shutdown(false);
}

void SingleThreadedJobProcessor::shutdown(bool terminate){	
	{
		unique_lock<mutex> lk(m);
		status = OperationalStatus::SHUTTING_DOWN;
		if (terminate){
				abortPendingJobs();
		}

		cv.notify_one();		
	}
	if(myThread != nullptr){
		// maybe the thread has never been started.
		myThread->join();
		delete(myThread);
		myThread = nullptr;
	}
}

SingleThreadedJobProcessor::~SingleThreadedJobProcessor(){
	if ( status == OperationalStatus::OPERATIONAL ){
		shutdown(true);
	}
}

void SingleThreadedJobProcessor::process(){
	monitoring_namespace_protect_thread();

	while(true){
		void * job;
		{
			unique_lock<mutex> lk(m);
			while( queue->empty() ){
				if ( status == OperationalStatus::SHUTTING_DOWN ){
					return;
				}
				cv.wait(lk);
			}
			job = queue->pollNextJob();
		}
		processJob(job);
	}
}

}