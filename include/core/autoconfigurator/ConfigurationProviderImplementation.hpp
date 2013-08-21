#include <core/autoconfigurator/ConfigurationProvider.hpp>

//XXX: This is needed to get the COMPONENT_INSTANCIATOR_NAME() macro. Should we move it to some other place so that the configurationProvider interface does not have to depend on Component.hpp?
#include <core/component/Component.hpp>

#define CONFIGURATION_PROVIDER_INSTANCIATOR_NAME COMPONENT_INSTANCIATOR_NAME(core_configuration_provider)

