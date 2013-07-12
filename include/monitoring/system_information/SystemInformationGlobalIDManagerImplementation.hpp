#ifndef __SYSTEMINFO_IMPL_H
#define __SYSTEMINFO_IMPL_H

#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>



// Module interfaces 

#define COMPONENT(x) \
extern "C"{\
void * get_instance_monitoring_systeminformation_global_id() { return new x(); }\
}

#endif