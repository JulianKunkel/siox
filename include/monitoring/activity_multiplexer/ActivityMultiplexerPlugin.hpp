#ifndef ACTIVITYMULTIPLEXER_PLUGIN_H
#define ACTIVITYMULTIPLEXER_PLUGIN_H

#include <assert.h>

#include <core/component/Component.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityPluginDereferencing.hpp>

using namespace core;

namespace monitoring{

class ActivityMultiplexerPlugin: public Component{
protected:
	ActivityMultiplexer * parent_multiplexer;
	ActivityPluginDereferencing * dereferenceFacade;

	virtual void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer & multiplexer) = 0;

public:
	void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer * activity_multiplexer, ActivityPluginDereferencing * dereferenceFacade){
		parent_multiplexer = activity_multiplexer;
		// may be 0.
		this->dereferenceFacade = dereferenceFacade;

		init(options, *parent_multiplexer );		
	}

	void init(ComponentOptions * options){
		ActivityMultiplexerPluginOptions * o = (ActivityMultiplexerPluginOptions *) options;
		assert(options != nullptr);
		assert(o->multiplexer.componentID != 0);

		init(o, o->multiplexer.instance<ActivityMultiplexer>(), o->dereferenceFacade.instance<ActivityPluginDereferencing>());
	}

	void shutdown(){
	}
};

}

#define ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE "monitoring_activitymultiplexer_plugin"

#endif

// BUILD_TEST_INTERFACE monitoring/activity_multiplexer/plugins/ActivityMultiplexerPlugin
