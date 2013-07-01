#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>

#include <core/module/module-loader.hpp>
#include <assert.h>

#include "siox-ll-internal.hpp"

using namespace std;

using namespace core;
using namespace monitoring;


/**
 * Implementation of the low-level API
 * The C-implementation serves as lightweight wrapper for C++ classes.
 */


///////////////// GLOBAL VARIABLES ///////////////////////

// variable indiciating if SIOX is finalized and needs initalization.
bool finalized = true;

struct process_info process_data = {0};

//////////////////////////////////////////////////////////



// Functions used for testing the interfaces.

ProcessID build_process_id(HwID hw, uint32_t pid, uint32_t time){
	return {.hw = hw, .pid = pid, .time = time};
}


//////////////////////////////////////////////////////////

HwID lookup_hardware_id(){
	// Dummy implementation	
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	// TODO contact daemon / SIOX kb to get unique ID based on the hostname.
	return 1;	
}

// Each process can create a runtime ID locally KNOWING the HwID from the daemon
ProcessID create_process_id(HwID hw){
	pid_t pid = getpid();
	struct timespec tv;
    
    clock_gettime(CLOCK_MONOTONIC, & tv);

	// casting will strip of the most significant bits on X86 and hence preserve the current seconds and PID
	return {.hw = hw, .pid = (uint32_t) pid , .time = (uint32_t) tv.tv_sec};
}



extern "C"{


// constructor for the shared library
__attribute__ ((constructor)) void siox_ll_ctor()
{
	// load local configuration if possible.
	siox_process_register(NULL, NULL);
}

// destructor for the shared library
__attribute__ ((destructor)) void siox_ll_dtor()
{
    siox_process_finalize();
}


///////////////////// Implementation of SIOX-LL /////////////

void siox_process_register(HwID * hwid, ProcessID * pid){ // , const char * configuration_parameters?
	if (hwid != NULL){
		process_data.hwid = *hwid;
	}else{
		process_data.hwid = lookup_hardware_id();
	}

	if(pid != NULL){	
		process_data.pid = *pid;
	}else{
		process_data.pid = create_process_id(process_data.hwid);
	}

	if(finalized){
		printf("Initializing SIOX library\n");

		// Lookup and initialize configurations based on hwid and pid...
		// Load required modules and pull the interfaces into global datastructures
		// Use an environmentvariable and/or configuration files in <DIR> or /etc/siox.conf
		// TODO 
		// For the moment use the file ontology module.		

		Ontology * o = module_create_instance<Ontology>("", "FileOntology", ONTOLOGY_INTERFACE);
		process_data.ontology = o;
		ComponentOptions * options = o->get_options();
		// Init using an empty configuration (for now).
		o->init(options);

	}

	finalized = false;
}

void siox_process_finalize(){
	if (finalized){
		return;
	}

	// cleanup datastructures etc.
	// TODO

	// cleanup ontology
	process_data.ontology->shutdown();
	delete(process_data.ontology);
	process_data.ontology = NULL;

	finalized = true;
}



// copy value based on datatype in the attribute
static Value convert_attribute(siox_attribute * attribute, void * value){
	Value v;
	switch(attribute->storage_type){
	case(SIOX_STORAGE_32_BIT_UINTEGER):
    case(SIOX_STORAGE_32_BIT_INTEGER):{
    	v = *((int32_t * ) value);
    	break;
    }
    case(SIOX_STORAGE_64_BIT_UINTEGER):
    case(SIOX_STORAGE_64_BIT_INTEGER):{
    	v = *((int64_t * ) value);    	
    	break;
    }
    case(SIOX_STORAGE_FLOAT):{
    	v = *((float * ) value);
    	break;
    }
    case(SIOX_STORAGE_DOUBLE):{
    	v = *((double * ) value);
    	break;
    }
    case(SIOX_STORAGE_STRING):{
    	break;
    }
    case (SIOX_STORAGE_UNASSIGNED):{
    	assert(0);
    }
    }
    return v;
}




void siox_process_set_attribute(siox_attribute * attribute, void * value){
	assert(attribute != NULL);
	assert(value != NULL);

	// TODO propose value into monitoring path
}

/////////////// MONITORING /////////////////////////////
siox_component * siox_component_register(UniqueInterfaceID * uid, const char * iid){
	assert(uid != NULL);
	// iid could be NULL

	// TODO
	return NULL;
}

void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value){
	assert(attribute != NULL);
	assert(value != NULL);

	// TODO
}


//////////////// ONTOLOGY  ///////////////////////////////////

siox_attribute * siox_attribute_register(const char * name, enum siox_ont_storage_type storage_type){
	assert(name != NULL);

	string s(name);
	return process_data.ontology->attribute_register(s, storage_type);
}


void siox_metric_set_attribute(siox_metric * metric, siox_attribute * attribute, void * value){	
	assert(metric != NULL);
	assert(attribute != NULL);
	assert(value != NULL);

	Value v = convert_attribute(attribute, value);
	process_data.ontology->metric_set_attribute(metric, attribute, v);
}


siox_metric * siox_ontology_register_metric(const char *                canonical_name,
                                            const char *                unit,
                                            enum siox_ont_storage_type  storage)
{
	assert(canonical_name != NULL);
	assert(unit != NULL);

	string n(canonical_name);
	string u(unit);
	return process_data.ontology->register_metric(n, u, storage);
}

siox_metric * siox_ontology_find_metric_by_name( const char * canonical_name){
	assert(canonical_name != NULL);

	string n(canonical_name);
	return process_data.ontology->find_metric_by_name(n);
}

}
