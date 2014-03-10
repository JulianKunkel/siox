#ifndef ACTIVITY_MULTIPLEXER_SERIALIZATION_PLUGIN_H
#define ACTIVITY_MULTIPLEXER_SERIALIZATION_PLUGIN_H

#include <monitoring/datatypes/Activity.hpp>
#include <core/module/Module.hpp>

namespace monitoring{

/* 
 This interface allows to de-serialize stored data.
 */
class ActivitySerializationPlugin : public core::ModuleInterface{
public:
	virtual void loadTrace(string configEntry) = 0;
	virtual void closeTrace() = 0;
	virtual Activity * nextActivity() = 0;
	virtual bool hasNextActivity() = 0;

};

}

#define ACTIVITY_SERIALIZATION_PLUGIN_INTERFACE "monitoring_activity_serialization_plugin"

#endif