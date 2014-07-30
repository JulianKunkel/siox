#include <map>
#include <string>

#include <monitoring/activity_multiplexer/plugins/profiler/ActivityProfiler.hpp>
#include <monitoring/activity_multiplexer/plugins/profiler/ActivityProfilerPluginOptions.hpp>

#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>

#include <util/time.h>

using namespace std;

ComponentReport ActivityProfilerPlugin::prepareReport(){
	ComponentReport report;

	if (facade == nullptr || facade->get_system_information() == nullptr ){
		return report;
	}

	SystemInformationGlobalIDManager * sinfo = facade->get_system_information();

	struct Perf_Groups{
		GroupEntry * runTimeSpend;
		GroupEntry * calls;

		Perf_Groups(const string & name){
			GroupEntry * p = new GroupEntry( name );
			runTimeSpend = new GroupEntry("time", p);
			calls = new GroupEntry("calls", p);
		}
	};

	auto m = map<string, Perf_Groups>();

	for(auto it = statistics.begin(); it != statistics.end(); it++){
			struct ActivityStatistic & stats = it->second;

			UniqueInterfaceID uiid = sinfo->lookup_interface_of_activity(it->first);
			string activityName = sinfo->lookup_activity_name(it->first);
			string interfaceName = sinfo->lookup_interface_name(uiid);

			//cout << activityName << ": " << stats.calls << " " << siox_time_in_s(stats.runTimeSpend) << " s" << endl;
		
			if ( m.find(interfaceName) == m.end() ) {
				m.insert(pair<string, Perf_Groups>(interfaceName, Perf_Groups(interfaceName)));
			}
			struct Perf_Groups & perf = m.find(interfaceName)->second;

			report.addEntry( new GroupEntry(activityName, perf.calls) , ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE,  stats.calls ));
			report.addEntry( new GroupEntry(activityName, perf.runTimeSpend) , ReportEntry( ReportEntry::Type::APPLICATION_PERFORMANCE,  stats.runTimeSpend ));
	}

	return report;
}

void ActivityProfilerPlugin::stop(){
	// Push the statistics upstream if needed.
	// TODO
}

void ActivityProfilerPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	//activity.duration
	lock_guard<boost::detail::spinlock> guard(lock);
	struct ActivityStatistic & stats = statistics[activity->ucaid()];
	stats.calls++;
	stats.runTimeSpend += activity->duration();
}

ComponentOptions * ActivityProfilerPlugin::AvailableOptions() {
	return new ActivityProfilerPluginOptions();
}

void ActivityProfilerPlugin::initPlugin( ) {
	ActivityProfilerPluginOptions & o = getOptions<ActivityProfilerPluginOptions>();
	assert(multiplexer);

	multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( & ActivityProfilerPlugin::Notify ), false );
}

void ActivityProfilerPlugin::finalize() {
	multiplexer->unregisterCatchall( this, false );
	ActivityMultiplexerPlugin::finalize();
}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityProfilerPlugin();
	}
}
