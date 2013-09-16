#ifndef STATISTICS_MULTIPLEXER_PLUGIN_OPTIONS
#define STATISTICS_MULTIPLEXER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>


using namespace core;

namespace monitoring {

	//@serializable
	class StatisticsMultiplexerPluginOptions: public ComponentOptions {
		public:
			ComponentReference multiplexer;
			ComponentReference dereferenceFacade;
	};

}


#endif
