#ifndef SIOX_ONTOLOGY_IMPL_H
#define SIOX_ONTOLOGY_IMPL_H

#include <monitoring/ontology/Ontology.hpp>


// Module interfaces

#define COMPONENT(x) \
	extern "C"{\
		void * get_instance_monitoring_ontology() { return new x(); }\
	}




#endif
