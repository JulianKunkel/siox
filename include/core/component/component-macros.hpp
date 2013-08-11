#ifndef SIOX_COMPONENT_MACROS_H
#define SIOX_COMPONENT_MACROS_H

#include <core/component/Component.hpp>
#include <core/container/container-macros.hpp>


#define SERIALIZE_CONTAINER(VAR_) SERIALIZE(VAR_ PARENT_CLASS(Container))

#endif
