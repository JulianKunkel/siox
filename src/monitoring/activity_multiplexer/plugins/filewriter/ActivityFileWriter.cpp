#include <iostream>
#include <fstream>
#include <list>
#include <mutex>
#include <sstream>

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

		void Notify( const shared_ptr<Activity> & activity, int lost ) {
			//cout << "NOTIFY" << facade->get_system_information()->lookup_activity_name( activity->ucaid_) << endl;
			//if (  activity->attributeArray().begin() != activity->attributeArray().end() )
			//	cout << activity->attributeArray().begin()->value << endl;
			Activity * act = &* activity;

			lock_guard<mutex> lock( motify_mutex );
			*oa << act;
			if( synchronize )
				file.flush();
		}

		ComponentOptions * AvailableOptions() override {
			return new FileWriterPluginOptions();
		}

		void initPlugin( ) override {
			FileWriterPluginOptions & o = getOptions<FileWriterPluginOptions>();
   	 	
   	 	stringstream buff;
   	 	buff<< o.filename;
   	 	if (! getenv("SIOX_ACTIVITY_WRITER_FILENAME_DONT_APPEND_PID")){
    			buff << (long long unsigned) getpid();
     		}			
			file.open( buff.str() );
			oa = new boost::archive::text_oarchive( file, boost::archive::no_header | boost::archive::no_codecvt );

			synchronize = o.synchronize;
			multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &FileWriterPlugin::Notify ), false );
		}

		void finalize() override {
			multiplexer->unregisterCatchall( this, false );
			ActivityMultiplexerPlugin::finalize();
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
