#ifndef STATISTICS_MULTIPLEXER_PLUGIN_OPTIONS
#define STATISTICS_MULTIPLEXER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

namespace monitoring {

	//@serializable
	class StatisticsMultiplexerPluginOptions: public core::ComponentOptions {
		public:
			core::ComponentReference multiplexer;
			core::ComponentReference dereferencingFacade;
	};

}


#endif
