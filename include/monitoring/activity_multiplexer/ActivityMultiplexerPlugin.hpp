#ifndef ACTIVITYMULTIPLEXER_PLUGIN_H
#define ACTIVITYMULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>


using namespace core;

namespace monitoring{


class ActivityMultiplexerPluginOptions: public ComponentOptions{
// protected:
// If needed: prevent accidential creation of object
//	ActivityMultiplexerPluginOptions(){}
public:
	// must be provided
	ActivityMultiplexer * multiplexer = 0;
};

class ActivityMultiplexerPlugin: public Component{
protected:
	ActivityMultiplexer * parent_multiplexer;

	virtual void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer & multiplexer) = 0;

public:
	void init(ComponentOptions * options){
		ActivityMultiplexerPluginOptions * o = (ActivityMultiplexerPluginOptions *) options;
		assert(options != nullptr);
		assert(o->multiplexer != nullptr);
		init(o, * (o->multiplexer) );
	}

	void shutdown(){
	}

};

}

#define ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_activitymultiplexer_plugin"

#endif

// BUILD_TEST_INTERFACE monitoring/activity_multiplexer/plugins/ActivityMultiplexerPlugin
