#ifndef THREADED_STATISTICS_PROVIDER_PLUGIN_OPTIONS
#define THREADED_STATISTICS_PROVIDER_PLUGIN_OPTIONS

#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring{


class ThreadedStatisticsProviderPluginOptions: public ComponentOptions{
public:
	ComponentReference ThreadedstatisticsCollector;

	SERIALIZE_CONTAINER(MEMBER_INJECTION(ThreadedstatisticsCollector))
};

}

#define SERIALIZE_OPTIONS(VAR_) SERIALIZE(VAR_ PARENT_CLASS(ThreadedStatisticsProviderPluginOptions))

#endif
