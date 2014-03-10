#ifndef MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BINWRITER_HPP
#define MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BINWRITER_HPP

#include <stdio.h>
#include <errno.h>
#include <mutex>


#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/activity_multiplexer/ActivitySerializationPlugin.hpp>

using namespace std;
using namespace monitoring;
using namespace core;


class ActivityBinWriterPlugin: public ActivityMultiplexerPlugin{
	private:
		FILE * file = nullptr;
		mutex notify_mutex;
	public:

		void Notify( const shared_ptr<Activity> & activity, int lost );

		ComponentOptions * AvailableOptions() override;

		void initPlugin( ) override;

		void finalize() override;

		~ActivityBinWriterPlugin();
};

/*
Class for reading a trace back. NOT thread-safe.
 */
class ActivityTraceReaderPlugin: public ActivitySerializationPlugin{
public:	
	void loadTrace(string configEntry) override;
	void closeTrace() override;
	Activity * nextActivity() override;
	bool hasNextActivity() override;
private:
		FILE * file = nullptr;
};

#endif
