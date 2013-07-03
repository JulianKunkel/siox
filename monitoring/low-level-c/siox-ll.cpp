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

NodeID lookup_hardware_id(const char * hostname){
    // Dummy implementation 
    if ( hostname == NULL)
    {
        // TODO: Adapt this to C++?
        char hostname[1024];
        hostname[1023] = '\0';
        gethostname(hostname, 1023);
    }

    // TODO contact daemon / SIOX kb to get unique ID based on the hostname.
    return process_data.system_information_manager->node_id(hostname);
}


// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
ProcessID create_process_id(NodeID hw){
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

void siox_process_register(NodeID * nodeID, ProcessID * pid){ // , const char * configuration_parameters?


    if (nodeID != NULL){
        process_data.nodeID = *nodeID;
    }else{
        process_data.nodeID = lookup_hardware_id();
    }

    if(pid != NULL){    
        process_data.pid = *pid;
    }else{
        process_data.pid = create_process_id(process_data.nodeID);
    }

    if(finalized){
        printf("Initializing SIOX library\n");

        // Lookup and initialize configurations based on nodeID and pid...
        // Load required modules and pull the interfaces into global datastructures
        // Use an environment variable and/or configuration files in <DIR> or /etc/siox.conf
        // TODO 
        // For the moment use the file ontology module.     

        // Acquire reference to a fully configured Ontology object
        Ontology * o = module_create_instance<Ontology>("", "FileOntology", ONTOLOGY_INTERFACE);
        ComponentOptions * o_options = o->get_options();
        // Init using an empty configuration (for now).
        o->init(o_options);
        process_data.ontology = o;

        // Acquire reference to a fully configured SystemInformationGlobalIDManager object
        // TODO: Set correct parameters for this call!!!
        SystemInformationGlobalIDManager * sigidm = module_create_instance<SystemInformationGlobalIDManager>("", "FileOntology", ONTOLOGY_INTERFACE);
        // TODO: Depending on how o_options was used, we may make do
        // without a fresh ComponentOtions object
        ComponentOptions * sigidm_options = sigidm->get_options();
        // Init using an empty configuration (for now).
        sigidm->init(sigidm_options);
        process_data.system_information_manager = sigidm;

        // Acquire reference to a fully configured AssociationMapper object
        // TODO: Set correct parameters for this call!!!
        AssociationMapper * am = module_create_instance<AssociationMapper>("", "FileOntology", ONTOLOGY_INTERFACE);
        // TODO: Depending on how o_options was used, we may make do
        // without a fresh ComponentOtions object
        ComponentOptions * sigidm_options = am->get_options();
        // Init using an empty configuration (for now).
        am->init(sigidm_options);
        process_data.association_mapper = am;


        //TODO: Hier andere "globale" Objekte holen und init'en!
        // Information providers and tools, to be acquired ASAP!
        // ActivityBuilder, AMux, SMUx, ...
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

    // cleanup system_information_manager
    process_data.system_information_manager->shutdown();
    delete(process_data.system_information_manager);
    process_data.system_information_manager = NULL;

	// cleanup association_mapper
	process_data.association_mapper->shutdown();
	delete(process_data.association_mapper);
	process_data.association_mapper = NULL;

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

    Attribute att(attribute);
    Value val = convert_attribute(attribute, value);
    process_data.association_mapper->set_process_attribute(process_data.pid, attribute, val);

	// TODO propose value into monitoring path
    // MZ: ?!?
}


RemoteInstanceID * siox_associate_instance(const char * iid){
    string instance(iid);
    return process_data.association_mapper->create_instance_mapping(instance);
}

/////////////// MONITORING /////////////////////////////

// Signaturen C-verdaulich anpassen!!

siox_component * siox_component_register(UniqueInterfaceID * uiid, const char * instance_name){
    assert(uiid != NULL);
    // instance_name could be NULL

    // TODO
    return NULL;
}


void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value){
    assert(attribute != NULL);
    assert(value != NULL);

    ComponentID cid(component);
    Attribute att(attribute);
    Value val = convert_attribute(attribute, value);
    process_data.association_mapper->set_component_attribute(cid, att, val);
}


siox_component_activity * siox_component_register_activity(UniqueInterfaceID * uiid, const char * activity_name){
// MZ: Wo denn nun registrieren - o, am oder sigidm?!?
    xyz.register_activity(uuid, activity_name);
}


void siox_component_unregister(siox_component * component){
    // MZ: Hat noch keine Zielfunktion!
    // TODO
}


void siox_report_node_statistics(NodeID hw, siox_attribute * statistic, siox_timestamp start_of_interval, siox_timestamp end_of_interval, void * value){

    // MZ: Das reicht eigentlich an den SMux weiter, oder?
    // TODO
}

/////////////// ACTIVITIES /////////////////////////////

// MZ: Alle an ActivityBuilder (to be acquired!!!) weiterreichen

siox_activity * siox_activity_start(siox_component_activity * activity, siox_timestamp timestamp){
}

void siox_activity_stop(siox_activity * activity, siox_timestamp timestamp){

}

void siox_activity_set_attribute(siox_activity * activity, siox_attribute * attribute, void * value){

}

void siox_activity_report_error(siox_activity * activity, int error){

}

void siox_activity_end(siox_activity * activity){
    // Fertige Activity von ab abfordern und an AMux (objekt?!?) weiterreichen

}

void siox_activity_link_to_parent(siox_activity * activity_child, siox_activity * activity_parent){

}

//////////////// REMOTE CALL  ///////////////////////////////////

siox_remote_call * siox_remote_call_start(siox_activity     * activity,
                                          NodeID                * target_NodeID,
                                          UniqueInterfaceID * target_uid,
                                          AssociateID       * target_iid){

}

void siox_remote_call_set_attribute(siox_remote_call * remote_call, siox_attribute * attribute, void * value){

}

void siox_remote_call_submitted(siox_remote_call * remote_call)…{

}

void siox_activity_started_by_remote_call(siox_activity * activity, NodeID * caller_NodeID_if_known, UniqueInterfaceID * caller_uid_if_known, AssociateID * caller_instance_if_known){

}

//////////////// ONTOLOGY  ///////////////////////////////////

siox_attribute * siox_ontology_register_attribute(const char * domain, const char * name, enum siox_ont_storage_type storage_type){
	assert(name != NULL);

	string s(name);
	return process_data.ontology->register_attribute(domain, name, storage_type);
}

int siox_ontology_set_meta_attribute(siox_attribute * parent, siox_attribute * meta_attribute, void * value){
    // MZ: How to handle value?!?
    process_data.ontology->attribute_set_meta_attribute(parent, meta_attribute, value);
    return 1; // MZ: Oder return (Zeile darüber)?!?
}

/*void siox_metric_set_attribute(siox_metric * metric, siox_attribute * attribute, void * value){	
	assert(metric != NULL);
	assert(attribute != NULL);
	assert(value != NULL);

	Value v = convert_attribute(attribute, value);
	process_data.ontology->metric_set_attribute(metric, attribute, v);
}
*/

siox_attribute * siox_ontology_register_attribute_with_unit(const char * domain, const char * name,
                                            const char *                unit,
                                            enum siox_ont_storage_type  storage){
	assert(canonical_name != NULL);
	assert(unit != NULL);

    string d(domain);
    string n(canonical_name);
    string u(unit);
    siox_attribute result = process_data.ontology->register_attribute(d, n, storage);
    Attribute * unit_attribute = process_data.ontology->lookup_attribute_by_name(u);
    // MZ: process_data.ontology->attribute_set_meta_attribute(result, unit_attribute, ?!?);
}

siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name){
    assert(canonical_name != NULL);

    string d(domain);
	string n(canonical_name);
	return process_data.ontology->lookup_attribute_by_name(d, n);
}


UniqueInterfaceID * siox_system_information_lookup_interface_id(const char * interface_name, const char * implementation_identifier){
}
}
