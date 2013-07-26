#ifndef STATISTICS_COLLECTOR_PLUGIN_OPTIONS
#define STATISTICS_COLLECTOR_PLUGIN_OPTIONS
#include <core/component/component-macros.hpp>


using namespace core;

namespace monitoring{


class StatisticsCollectorOptions: public ComponentOptions{
public:
	ComponentReference smux;
	ComponentReference dereferingFacade;

	SERIALIZE_CONTAINER(MEMBER(smux) MEMBER(dereferingFacade))
};

}


#define SERIALIZE_OPTIONS(VAR_) SERIALIZE(VAR_ PARENT_CLASS(StatisticsCollectorOptions))

#endif
