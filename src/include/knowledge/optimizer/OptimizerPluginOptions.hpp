#ifndef OPTIMIZER_PLUGIN_OPTIONS
#define OPTIMIZER_PLUGIN_OPTIONS

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

//@serializable
class OptimizerPluginOptions: public core::ComponentOptions {
	public:
		core::ComponentReference optimizer;
		core::ComponentReference dereferenceFacade;
};

#endif
