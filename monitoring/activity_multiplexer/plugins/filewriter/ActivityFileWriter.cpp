#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/datatypes/ActivitySerializable.hpp>
#include <core/container/container-serializer-text.hpp>

#include "ActivityFileWriterOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class FileWriterPlugin: public ActivityMultiplexerPlugin, public ActivityMultiplexerListenerSync {
private:
	FileSerializer<ActivitySerializable> * serializer = nullptr;
public:
	void Notify(Activity * activity)
	{ 
		ActivitySerializable * a = (ActivitySerializable*)  activity;
		serializer->append(*a);
	}

	ComponentOptions * get_options(){
		return new FileWriterPluginOptions();
	}

	void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer& multiplexer){
		FileWriterPluginOptions * o = (FileWriterPluginOptions*) options;
		serializer = new FileSerializer<ActivitySerializable>(o->filename);
		multiplexer.registerListener(this);
	}

	~FileWriterPlugin(){
		delete(serializer);
	}
};

PLUGIN(FileWriterPlugin)

