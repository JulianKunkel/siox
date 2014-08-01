#ifndef MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_PROFILER_HPP
#define MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_PROFILER_HPP

#include <assert.h>

#include <map>
#include <boost/smart_ptr/detail/spinlock.hpp>


#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


using namespace std;
using namespace monitoring;
using namespace core;

/*
 This plugin computes a flat profile for all given activities and provides them in reports.
 */
class ActivityProfilerPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface {
private:
	boost::detail::spinlock lock;

	struct ActivityStatistic{
		uint64_t calls = 0;
		Timestamp runTimeSpend = 0;
	};

	map<UniqueComponentActivityID, ActivityStatistic> statistics;
public:
	void Notify( const shared_ptr<Activity> & activity, int lost );

	ComponentReport prepareReport() override;

	ComponentOptions * AvailableOptions() override;

	void stop() override;

	void initPlugin( ) override;

	void finalize() override;
};

#endif