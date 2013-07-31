#ifndef KNOWLEDGE_OPTIMIZER_PLUGIN_IMPL_HPP
#define KNOWLEDGE_OPTIMIZER_PLUGIN_IMPL_HPP

#include <knowledge/optimizer/OptimizerPlugin.hpp>

#define COMPONENT(x) \
extern "C"{\
void * get_instance_knowledge_optimizer_plugin() { return new x(); }\
}

#endif
