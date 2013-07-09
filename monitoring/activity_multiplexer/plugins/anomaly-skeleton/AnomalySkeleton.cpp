#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "AnomalySkeletonOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

// Create an implementation of the options.
CREATE_SERIALIZEABLE_CLS(AnomalySkeletonOptions)

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class AnomalySkeleton: public ActivityMultiplexerPlugin, public ActivityMultiplexerListenerSync {
public:
	void Notify(Activity * activity)
	{ 
	}

	ComponentOptions * get_options(){
		return new AnomalySkeletonOptions();
	}

	void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer & multiplexer){
		//AnomalySkeletonOptions * o = (AnomalySkeletonOptions*) options;

		assert(this->dereferenceFacade != nullptr);
		//ActivityPluginDereferencing * apdf = this->dereferenceFacade;

		
		multiplexer.registerListener(this);
	}
};

PLUGIN(AnomalySkeleton)

