#ifndef MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BINWRITER_HPP
#define MONITORING_PLUGINS_ACTIVITY_MULTIPLEXER_BINWRITER_HPP

#include <stdio.h>
#include <errno.h>


#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


using namespace std;
using namespace monitoring;
using namespace core;


class ActivityBinWriterPlugin: public ActivityMultiplexerPlugin {
	private:
		FILE * file = nullptr;
	public:

		void Notify( const shared_ptr<Activity> & activity, int lost );

		ComponentOptions * AvailableOptions() override;

		void initPlugin( ) override;

		void finalize() override;

		~ActivityBinWriterPlugin();
};

#endif