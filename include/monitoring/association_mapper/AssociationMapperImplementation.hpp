#ifndef __ASSOCIATION_MAPPER_BACKEND_H
#define __ASSOCIATION_MAPPER_BACKEND_H

#include <monitoring/association_mapper/AssociationMapper.hpp>

#define COMPONENT(x) \
extern "C"{\
void * get_instance_monitoring_association_mapper() { return new x(); }\
}



#endif