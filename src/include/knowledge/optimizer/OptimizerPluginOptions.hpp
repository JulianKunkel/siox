#ifndef OPTIMIZER_PLUGIN_OPTIONS
#define OPTIMIZER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>


using namespace core;

namespace knowledge {

	//@serializable
	class OptimizerPluginOptions: public ComponentOptions {
		public:
			ComponentReference optimizer;
			ComponentReference dereferenceFacade;
	};

}

#endif
