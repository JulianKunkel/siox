#ifndef KNOWLEDGE_OPTIMIZER_IMPL_HPP
#define KNOWLEDGE_OPTIMIZER_IMPL_HPP

#include <knowledge/optimizer/Optimizer.hpp>

#define COMPONENT(x) \
	extern "C"{\
		void * get_instance_knowledge_optimizer() { return new x(); }\
	}

#endif
