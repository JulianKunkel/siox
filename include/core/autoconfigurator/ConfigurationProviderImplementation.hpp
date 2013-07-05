#include <core/autoconfigurator/ConfigurationProvider.hpp>

#define COMPONENT(x) \
extern "C"{\
void * get_instance_core_configuration_provider() { return new x(); }\
}

