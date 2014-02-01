#include <unistd.h>

#include <iostream>
#include <thread>
#include <atomic>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include <util/threadSafety.h>

#include "ActivityMonitorOptions.hpp"

using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;

/*
 This class outputs the number of observed activities every second.
 */
class ActivityMonitor: public ActivityMultiplexerPlugin {
	thread * reporterThread = nullptr;
	
	bool terminate = false;

	atomic<uint64_t> observedActivities;
	atomic<uint64_t> droppedActivities;
	atomic<uint64_t> observedAsyncActivities;

	void reporterThreadFunc(){
		monitoring_namespace_protect_thread();
		
		uint64_t o[3] = {0,0,0};

		while(! terminate){
			uint64_t n[3];
			sleep (1);

			n[0] = droppedActivities.load();
			n[1] = observedActivities.load();
			n[2] = observedAsyncActivities.load();

			cout << "ActivityMonitor sync: " << (n[1] - o[1])  << " async: " << (n[2] - o[2]) << " dropped: " << (n[0] - o[0]) << " = " <<  100.0*(n[0] - o[0]) / (n[1] - o[1]) << " %" << endl;

			o[0] = n[0];
			o[1] = n[1];
			o[2] = n[2];
		}
	}

	public:
		void Notify( shared_ptr<Activity> activity ) override {
			observedActivities++;
		}

		void NotifyAsync( int lost_count, shared_ptr<Activity> activity ) override {
			observedAsyncActivities++;
			if( lost_count > 0 ){
				droppedActivities.fetch_add(lost_count);
			}
		}

		ComponentOptions * AvailableOptions() {
			return new ActivityMonitorOptions();
		}

		void initPlugin() {			
			if (reporterThread) return;

			observedActivities = 0;
			droppedActivities = 0;
			observedAsyncActivities = 0;

			reporterThread = new thread( & ActivityMonitor::reporterThreadFunc, this);
		}

		~ActivityMonitor(){
			if(reporterThread){
				terminate = true;
				reporterThread->join();
				delete(reporterThread);
			}
		}
};



extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityMonitor();
	}
}
