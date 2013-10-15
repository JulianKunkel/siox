#ifndef CORE_COMM_SERVICE_HPP_
#define CORE_COMM_SERVICE_HPP_

#include <core/component/Component.hpp>
#include <core/comm/CommunicationModule.hpp>

typedef core::Component NetworkService;

#define NETWORK_SERVICE_INTERFACE "network_service"

#define NETWORK_SERVICE_INSTANCIATOR_NAME MODULE_INSTANCIATOR_NAME(network_service)


#endif