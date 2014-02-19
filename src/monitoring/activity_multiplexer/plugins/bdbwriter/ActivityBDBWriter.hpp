#ifndef MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BDBWRITER_HPP
#define MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BDBWRITER_HPP

#include <db.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


using namespace std;
using namespace monitoring;
using namespace core;


class ActivityBDBWriterPlugin: public ActivityMultiplexerPlugin {
	private:
		DB_ENV *dbenv  = nullptr;
		DB * dbp = nullptr;
	public:

		void Notify( const shared_ptr<Activity> & activity, int lost );

		ComponentOptions * AvailableOptions() override;

		void initPlugin( ) override;

		void finalize() override;

		~ActivityBDBWriterPlugin();
};

#endif