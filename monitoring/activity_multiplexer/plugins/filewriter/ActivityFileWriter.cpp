#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
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
		serializer->append((ActivitySerializable *) activity);
	}

	ComponentOptions * AvailableOptions(){
		return new FileWriterPluginOptions();
	}

	void init(ActivityMultiplexer & multiplexer){
		FileWriterPluginOptions & o = getOptions<FileWriterPluginOptions>();
		serializer = new FileSerializer<ActivitySerializable>(o.filename);
		
		multiplexer.registerListener(this);
	}

	~FileWriterPlugin(){
		delete(serializer);
	}
};

CREATE_SERIALIZEABLE_CLS(FileWriterPluginOptions)

PLUGIN(FileWriterPlugin)

