#include <iostream>
#include <fstream>
#include <list>
#include <mutex>

#define TEXT_SERIALIZATION

#include <boost/archive/text_oarchive.hpp>

#include <core/container/container-serializable.hpp>

#include <monitoring/datatypes/ActivitySerializable.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>


#include "ActivityFileWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class FileWriterPlugin: public ActivityMultiplexerPlugin, public ActivityMultiplexerListener {
	private:
		ofstream file;
		bool synchronize;

		boost::archive::text_oarchive * oa;

		mutex motify_mutex;
	public:

		void Notify( Activity * activity ) {
			lock_guard<mutex> lock( motify_mutex );
			ActivityAccessor * as = reinterpret_cast<ActivityAccessor *>( activity );
			*oa << as;
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

			multiplexer->registerListener( this );
		}

		~FileWriterPlugin() {
			multiplexer->unregisterListener( this );

			file.close();
			delete( oa );
		}
};

PLUGIN( FileWriterPlugin )

