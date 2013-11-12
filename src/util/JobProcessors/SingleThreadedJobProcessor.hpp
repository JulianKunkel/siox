#ifndef UTIL_JOB_PROCESSORS_SINGLETHREADED_HPP
#define UTIL_JOB_PROCESSORS_SINGLETHREADED_HPP

#include <mutex>
#include <condition_variable>
#include <thread>

#include <util/JobProcessor.hpp>


namespace util{

/*
Create a single thread to process the work items.
 */
class SingleThreadedJobProcessor : public JobProcessor{
protected:
	std::mutex m;
	std::condition_variable cv;
	std::thread * myThread = nullptr;

	enum class OperationalStatus : uint8_t{
		OPERATIONAL = 0,
		SHUTTING_DOWN = 1,
	};

	OperationalStatus status = OperationalStatus::OPERATIONAL;
public:
	virtual void iStartJob(void * job);

	virtual void iCancelJob(void * job);

	virtual void shutdown( bool terminate = false );

	virtual void startProcessing();

	~SingleThreadedJobProcessor();
	
protected:
	void process();
};

}

#endif