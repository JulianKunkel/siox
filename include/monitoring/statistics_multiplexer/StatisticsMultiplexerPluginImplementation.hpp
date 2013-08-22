#ifndef SIOX_MMULTIPLEXER_MODULE_PLUGIN_
#define SIOX_MMULTIPLEXER_MODULE_PLUGIN_
#include <core/component/ComponentReferenceSerializable.hpp>

#include <monitoring/statistics_multiplexer/StatisticsMultiplexerPlugin.hpp>

#define PLUGIN(x) \
	extern "C"{\
		void * get_instance_monitoring_statisticsmultiplexer_plugin() { return new x(); }\
	}

#endif
