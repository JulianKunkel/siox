#ifndef ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_
#define ACTIVITY_PLUGIN_DEREFERENCING_FACADE_OPTIONS_H_

#include <core/component/component-macros.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace std;
using namespace monitoring;

// You may or may not use this advanced options.
class ActivityPluginDereferencingFacadeOptions: public ActivityMultiplexerPluginOptions{
public:
	ComponentReference ontology;
	ComponentReference system_information_manager;
	ComponentReference association_mapper;
	ComponentReference reasoner;

	SERIALIZE_CONTAINER(MEMBER(ontology) MEMBER(system_information_manager) MEMBER(association_mapper) MEMBER(reasoner))
};


#endif