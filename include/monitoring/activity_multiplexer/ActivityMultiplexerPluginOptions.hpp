#ifndef ACTIVITY_MULTIPLEXER_PLUGIN_OPTIONS
#define ACTIVITY_MULTIPLEXER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace core;

namespace monitoring {

	//@serializable
	class ActivityMultiplexerPluginOptions: public ComponentOptions {
		public:
			ComponentReference multiplexer;
			ComponentReference dereferenceFacade;
	};

}

#endif
