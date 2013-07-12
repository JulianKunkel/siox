//////////////////////////////////////////////////////////
/// @file siox-ll.cpp
//////////////////////////////////////////////////////////
/// Implementation of the SIOX Low-Level API,
/// realised as a light-weight wrapper of C++ classes,
//////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <core/module/module-loader.hpp>
#include <assert.h>

#include "siox-ll-internal.hpp"

using namespace std;

using namespace core;
using namespace monitoring;


///////////////// GLOBAL VARIABLES ///////////////////////

/// Flag indiciating whether SIOX is finalized and needing initalization.
bool finalized = true;

/// Struct to hold references to global objects needed.
struct process_info process_data = {0};



//////////////////////////////////////////////////////////



// Functions used for testing the interfaces.
/*ProcessID build_process_id(NodeID hw, uint32_t pid, uint32_t time){
    return {.hw = hw, .pid = pid, .time = time};
}
*/

//////////////////////////////////////////////////////////

NodeID lookup_node_id(string & hostname){
    return process_data.system_information_manager->node_id(hostname);
}


// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
ProcessID create_process_id(NodeID nid){
    pid_t pid = getpid();
    struct timespec tv;
    
    clock_gettime(CLOCK_MONOTONIC, & tv);

    // casting will strip off the most significant bits on X86 and hence preserve the current seconds and PID
    ProcessID result = ProcessID();
    result.nid = nid;
    result.pid = (uint32_t) pid;
    result.time = (uint32_t) tv.tv_sec;
    return result;
//    return {.nid = nid, .pid = (uint32_t) pid , .time = (uint32_t) tv.tv_sec};
}



extern "C"{

// constructor for the shared library
__attribute__ ((constructor)) void siox_ll_ctor()
{
    // Retrieve hostname; NodeID and PID will follow once process_data is set up
    // TODO: Adapt this to C++?
    char local_hostname[1024];
    local_hostname[1023] = '\0';
    gethostname(local_hostname, 1023);
    string hostname(local_hostname);

    // If necessary, do actual initialisation
    if(finalized){
        printf("Initializing SIOX library\n");

        // MZ: Do we really need nid and pid for this?!?
        // Lookup and initialize configurations based on nid and pid...
        // Load required modules and pull the interfaces into global datastructures
        // Use an environment variable and/or configuration files in <DIR> or /etc/siox.conf
        process_data.registrar = new ComponentRegistrar();

        // Check relevant environment modules:
        {
        const char * provider = getenv("SIOX_CONFIGURATION_PROVIDER_MODULE");
        const char * path = getenv("SIOX_CONFIGURATION_PROVIDER_PATH");
        const char * configuration = getenv("SIOX_CONFIGURATION_PROVIDER_ENTRY_POINT");

        provider = (provider != nullptr) ? provider : "FileConfigurationProvider" ;
        path = (path != nullptr) ? path : "";        
        configuration = (configuration != nullptr) ? configuration :  "siox.conf:/etc/siox.conf" ;

        process_data.configurator = new AutoConfigurator(process_data.registrar, provider, path, configuration);


        char * configurationMode = getenv("SIOX_CONFIGURATION_PROVIDER_MODE");

        // hostname configurationMode (is optional)        
        stringstream configName;
        configName << hostname;
        if(configurationMode != nullptr){
            configName << " " << configurationMode;
        }

        vector<Component*> loadedComponents = process_data.configurator->LoadConfiguration("Process", configName.str());
        // if(loadedComponents == nullptr){ // MZ: Error, "==" not defined
        if(loadedComponents.empty()){
            cerr << "FATAL Invalid configuration set: " << endl; 
            cerr << "SIOX_CONFIGURATION_PROVIDER_MODULE=" << provider << endl;
            cerr << "SIOX_CONFIGURATION_PROVIDER_PATH=" << path << endl;
            cerr << "SIOX_CONFIGURATION_PROVIDER_ENTRY_POINT=" << configuration << endl;
            cerr << "SIOX_CONFIGURATION_PROVIDER_MODE=" << configurationMode << endl;
            // TODO use FATAL function somehow?
            exit(1);
        }

        // check loaded components and asssign them to the right struct elements.
        process_data.ontology = dynamic_cast<Ontology*>(loadedComponents[0]);
        process_data.system_information_manager =  dynamic_cast<SystemInformationGlobalIDManager*>(loadedComponents[0]);
        process_data.association_mapper =  dynamic_cast<AssociationMapper*>(loadedComponents[0]);

        // Retrieve NodeID and PID now that we have a valid SystemInformationManager
        process_data.nid = lookup_node_id(hostname);
        process_data.pid = create_process_id(process_data.nid);
        }
    }

    finalized = false;
}

// destructor for the shared library
__attribute__ ((destructor)) void siox_ll_dtor()
{
    if (finalized){
        return;
    }

    // cleanup datastructures etc.
    // TODO

    // cleanup ontology
    process_data.ontology->shutdown();
    delete(process_data.ontology);
    process_data.ontology = nullptr;

    // cleanup system_information_manager
    process_data.system_information_manager->shutdown();
    delete(process_data.system_information_manager);
    process_data.system_information_manager = nullptr;

    // cleanup association_mapper
    process_data.association_mapper->shutdown();
    delete(process_data.association_mapper);
    process_data.association_mapper = nullptr;

    finalized = true;
}


///////////////////// Implementation of SIOX-LL /////////////

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> AttributeValue;

/// Convert an attribute's value to the generic datatype used in the ontology.
/// @param attribute [in]
/// @param value [in]
/// @return
static OntologyValue convert_attribute(siox_attribute * attribute, void * value){
	OntologyValue v;
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


/// Set a global attribute for a process.
/// @param attribute [in]
/// @param value [in]
void siox_process_set_attribute(siox_attribute * attribute, void * value){
	assert(attribute != nullptr);
	assert(value != nullptr);

    OntologyValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_process_attribute( & process_data.pid, attribute, val);
}


/// Associate some instance information (such as IP port or thread ID) with the current invocation.
/// @param iid [in]
/// @return
siox_associate * siox_associate_instance(const char * iid){
    string instance(iid);
    uint64_t id = process_data.association_mapper->create_instance_mapping(instance);
    // Be aware that this cast is dangerous. For future extensionability this can be replaced with a struct etc.
    return (AssociateID*) id;
}

/////////////// MONITORING /////////////////////////////

/// 
/// @param uiid [in]
/// @param instance_name [in]
/// @return
siox_component * siox_component_register(UniqueInterfaceID * uiid, const char * instance_name){
    assert(uiid != nullptr);
    // instance_name could be nullptr

    // TODO
    return nullptr;
}


void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value){
    assert(attribute != nullptr);
    assert(value != nullptr);

    // MZ: TODO siox_component in ComponentID wandeln
    ComponentID * cid = nullptr; // FIXME
    OntologyValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_component_attribute(cid, attribute, val);
}


siox_component_activity * siox_component_register_activity(siox_unique_interface * uiid, const char * activity_name){
// MZ: Wo denn nun registrieren - o, am oder sigidm?!?
    // xyz.register_activity(uuid, activity_name);
    return nullptr;
}


void siox_component_unregister(siox_component * component){
    // MZ: Hat noch keine Zielfunktion!
    // TODO
}


void siox_report_node_statistics(siox_node node, siox_attribute * statistic, siox_timestamp start_of_interval, siox_timestamp end_of_interval, void * value){

    // MZ: Das reicht eigentlich an den SMux weiter, oder?
    // TODO
}

/////////////// ACTIVITIES /////////////////////////////

// MZ: Alle an ActivityBuilder (to be acquired!!!) weiterreichen

siox_activity * siox_activity_start(siox_component_activity * activity){
    return nullptr;
}

void siox_activity_stop(siox_activity * activity){

}

void siox_activity_set_attribute(siox_activity * activity, siox_attribute * attribute, void * value){

}

void siox_activity_report_error(siox_activity * activity, int64_t error){

}

void siox_activity_end(siox_activity * activity){
    // Fertige Activity von ab abfordern und an AMux (objekt?!?) weiterreichen

}

void siox_activity_link_to_parent(siox_activity * activity_child, siox_activity * activity_parent){

}

//////////////// REMOTE CALL ///////////////////////////////////

siox_remote_call * siox_remote_call_start(siox_activity         * activity,
                                          siox_node             * target_nid,
                                          siox_unique_interface * target_uid,
                                          siox_associate        * target_iid){
    // MZ: TODO Festlegen, was (wenn überhaupt etwas) einen Remote Call ausmacht!
    return nullptr;
}

void siox_remote_call_set_attribute(siox_remote_call * remote_call, siox_attribute * attribute, void * value){

}

void siox_remote_call_submitted(siox_remote_call * remote_call){

}

void siox_activity_started_by_remote_call(siox_activity * activity, NodeID * caller_NodeID_if_known, UniqueInterfaceID * caller_uid_if_known, AssociateID * caller_instance_if_known){

}

//////////////// ONTOLOGY  ///////////////////////////////////

siox_attribute * siox_ontology_register_attribute(const char * domain, const char * name, enum siox_ont_storage_type storage_type){
    assert(domain != nullptr);
	assert(name != nullptr);

    string d(domain);
	string n(name);
	return process_data.ontology->register_attribute(d, n, storage_type);
}


int siox_ontology_set_meta_attribute(siox_attribute * parent, siox_attribute * meta_attribute, void * value){
    Value val = convert_attribute(meta_attribute, value);
    return process_data.ontology->attribute_set_meta_attribute(parent, meta_attribute, val);
}

/*void siox_metric_set_attribute(siox_metric * metric, siox_attribute * attribute, void * value){	
	assert(metric != nullptr);
	assert(attribute != nullptr);
	assert(value != nullptr);

	OntologyValue v = convert_attribute(attribute, value);
	process_data.ontology->metric_set_attribute(metric, attribute, v);
}
*/

siox_attribute * siox_ontology_register_attribute_with_unit(const char * domain, const char * name,
                                            const char *                unit,
                                            enum siox_ont_storage_type  storage){
	assert(name != nullptr);
	assert(unit != nullptr);

    string d(domain);
    string n(name);
    string u(unit);
    OntologyAttribute * result = process_data.ontology->register_attribute(d, n, storage);
    // MZ: Hier besser eigene Domain für Units verwenden?!
    // OntologyAttribute * unit_attribute = process_data.ontology->lookup_attribute_by_name(d, u);
    // MZ: process_data.ontology->attribute_set_meta_attribute(result, unit_attribute, ?!?);

    return result;
}

siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name){
    assert(name != nullptr);

    string d(domain);
	string n(name);
	return process_data.ontology->lookup_attribute_by_name(d, n);
}


siox_unique_interface * siox_system_information_lookup_interface_id(const char * interface_name, const char * implementation_identifier){
    return nullptr;
}
}
