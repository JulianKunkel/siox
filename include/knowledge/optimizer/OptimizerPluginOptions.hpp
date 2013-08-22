#ifndef OPTIMIZER_PLUGIN_OPTIONS
#define OPTIMIZER_PLUGIN_OPTIONS

#include <core/component/component-macros.hpp>


using namespace core;

namespace knowledge {


	class OptimizerPluginOptions: public ComponentOptions {
		public:
			ComponentReference optimizer;
			ComponentReference dereferenceFacade;

			SERIALIZE_CONTAINER( MEMBER( optimizer ) MEMBER( dereferenceFacade ) )
	};

}


#define SERIALIZE_OPTIONS(VAR_) SERIALIZE(VAR_ PARENT_CLASS(OptimizerPluginOptions))

#endif
