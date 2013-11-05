#include <iostream>
#include <fstream>
#include <list>
#include <mutex>

#include <boost/archive/text_oarchive.hpp>

#include <monitoring/datatypes/Activity.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "ActivityFileWriterOptions.hpp"

#include <core/component/ActivitySerializableText.cpp>

using namespace std;
using namespace monitoring;
using namespace core;

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class FileWriterPlugin: public ActivityMultiplexerPlugin {
	private:
		ofstream file;
		bool synchronize;

		boost::archive::text_oarchive * oa;

		mutex motify_mutex;
	public:

		void Notify( shared_ptr<Activity> activity ) {
			lock_guard<mutex> lock( motify_mutex );
			Activity * act = &* activity;
			*oa << act;
			if( synchronize )
				file.flush();
		}

		ComponentOptions * AvailableOptions() {
			return new FileWriterPluginOptions();
		}

		void initPlugin( ) {
			FileWriterPluginOptions & o = getOptions<FileWriterPluginOptions>();
			file.open( o.filename );
			oa = new boost::archive::text_oarchive( file, boost::archive::no_header | boost::archive::no_codecvt );

			synchronize = o.synchronize;
		}

		~FileWriterPlugin() {
			file.close();
			delete( oa );
		}
};

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileWriterPlugin();
	}
}
