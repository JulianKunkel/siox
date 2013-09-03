#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;
using namespace monitoring;

// You may or may not use this advanced options.
//@serializable
class ActivityPluginDereferencingOptions: public ActivityMultiplexerPluginOptions {
	public:
		ComponentReference ontology;
		ComponentReference system_information_manager;
		ComponentReference association_mapper;
		ComponentReference reasoner;
};


#endif