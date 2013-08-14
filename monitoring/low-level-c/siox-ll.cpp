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
#include <assert.h>

#include <core/module/module-loader.hpp>
#include <core/datatypes/VariableDatatype.hpp>

#include "siox-ll-internal.hpp"

extern "C"{
#define SIOX_INTERNAL_USAGE
#include <C/siox.h>
}

using namespace std;
using namespace core;
using namespace monitoring;

#define U32_TO_P(i) ((void*)((size_t)(i)))
#define P_TO_U32(p) ((uint32_t)((size_t)(p)))


//############################################################################
///////////////// GLOBAL VARIABLES ///////////////////////
//############################################################################

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

NodeID lookup_node_id(const string & hostname){
    return process_data.system_information_manager->register_nodeID(hostname);
}


//////////////////////////////////////////////////////////////////////////////
/// Create a proces id object .
//////////////////////////////////////////////////////////////////////////////
/// @param nid [in] The node id of the hardware node the process runs on
//////////////////////////////////////////////////////////////////////////////
/// @return A process id, which is system-wide unique
//////////////////////////////////////////////////////////////////////////////
// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
ProcessID create_process_id(NodeID nid){
    pid_t pid = getpid();
    struct timespec tv;

#ifndef __APPLE__
    clock_gettime(CLOCK_MONOTONIC, & tv);
#endif

    // casting will strip off the most significant bits on X86 and hence preserve the current seconds and PID
    ProcessID result = ProcessID();
    result.nid = nid;
    result.pid = (uint32_t) pid;
    result.time = (uint32_t) tv.tv_sec;
    return result;
//    return {.nid = nid, .pid = (uint32_t) pid , .time = (uint32_t) tv.tv_sec};
}



extern "C"{

// Constructor for the shared library
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
        configuration = (configuration != nullptr) ? configuration :  "siox.conf:/etc/siox.conf:monitoring/low-level-c/test/siox.conf:monitoring/low-level-c/test/siox.conf" ;

        process_data.configurator = new AutoConfigurator(process_data.registrar, provider, path, configuration);

        const char * configurationMode = getenv("SIOX_CONFIGURATION_PROVIDER_MODE");
        const char * configurationOverride = getenv("SIOX_CONFIGURATION_SECTION_TO_USE");

        string configName;
        if (configurationOverride != nullptr){
            configName = configurationOverride;
        }else{
            // hostname configurationMode (is optional)        

        {
            stringstream configName_s;
            configName_s << hostname;
            if(configurationMode != nullptr){
                configName_s << " " << configurationMode;
            }
        configName = configName_s.str();
        }
    }

    cout << provider << " path " << path << " " << configuration << " ConfigName: \"" << configName << "\"" << endl;

    vector<Component*> loadedComponents;
    try{
        loadedComponents = process_data.configurator->LoadConfiguration("Process", configName);
    }catch(InvalidConfiguration& e){
        // fallback to global configuration
        loadedComponents = process_data.configurator->LoadConfiguration("Process", "");
    }
    

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

        // check loaded components and assign them to the right struct elements.
        process_data.ontology = process_data.configurator->searchFor<Ontology>(loadedComponents);
        process_data.system_information_manager = process_data.configurator->searchFor<SystemInformationGlobalIDManager>(loadedComponents);
        process_data.association_mapper =  process_data.configurator->searchFor<AssociationMapper>(loadedComponents);
        process_data.amux = process_data.configurator->searchFor<ActivityMultiplexer>(loadedComponents); 

        assert(process_data.ontology);
        assert(process_data.system_information_manager);
        assert(process_data.association_mapper);
        assert(process_data.amux);        
        }
    // Retrieve NodeID and PID now that we have a valid SystemInformationManager
    process_data.nid = lookup_node_id(hostname);
    process_data.pid = create_process_id(process_data.nid);
    }

    finalized = false;
}

// Destructor for the shared library
__attribute__ ((destructor)) void siox_ll_dtor()
{
    if (finalized){
        return;
    }

    // cleanup datastructures by using the component registrar:
    process_data.registrar->shutdown();

    delete(process_data.registrar);
    delete(process_data.configurator);

    finalized = true;
}

//############################################################################
///////////////////// Implementation of SIOX-LL /////////////
//############################################################################

/// Variant datatype to uniformly represent the multitude of different attibutes
typedef VariableDatatype AttributeValue;


//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
static VariableDatatype convert_attribute(siox_attribute * attribute, void * value){
    AttributeValue v;
    switch(attribute->storage_type){
    case(VariableDatatype::Type::UINT32):
        return *((uint32_t * ) value);
    case(VariableDatatype::Type::INT32):{
        return *((int32_t * ) value);
    }
    case(VariableDatatype::Type::UINT64):
        return *((uint64_t * ) value);      
    case(VariableDatatype::Type::INT64):{
        return *((int64_t * ) value);      
    }
    case(VariableDatatype::Type::FLOAT):{
       return  *((float * ) value);
    }
    case(VariableDatatype::Type::DOUBLE):{
       return  *((double * ) value);
    }
    case(VariableDatatype::Type::STRING):{
       return  (char *) value;
    }
    case (VariableDatatype::Type::INVALID):{
        assert(0);
    }
    }
}


void siox_process_set_attribute(siox_attribute * attribute, void * value){
    assert(attribute != nullptr);
    assert(value != nullptr);

    AttributeValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_process_attribute( process_data.pid, *attribute, val);
}


siox_associate * siox_associate_instance(const char * instance_information){
    return U32_TO_P(process_data.association_mapper->create_instance_mapping(instance_information));
}

//############################################################################
/////////////// MONITORING /////////////////////////////
//############################################################################

siox_node * siox_lookup_node_id( const char * hostname ){
    if(hostname == NULL){
        // we take the localhost's ID.
       return U32_TO_P(process_data.nid);
    }
    
    return U32_TO_P(lookup_node_id(hostname));
}


siox_component * siox_component_register(siox_unique_interface * uiid, const char * instance_name){
    assert(uiid != SIOX_INVALID_ID );
    assert(instance_name != nullptr);

    UniqueInterfaceID uid = P_TO_U32(uiid);
    const string & interface_implementation = process_data.system_information_manager->lookup_interface_implementation(uid);
    const string & interface_name = process_data.system_information_manager->lookup_interface_name(uid);


    char configName[1001];
    snprintf(configName, 1000, "%s %s", interface_name.c_str(), interface_implementation.c_str());

    cout << "siox_component_register: " << configName << endl;


    vector<Component*> loadedComponents;
    try{
        loadedComponents = process_data.configurator->LoadConfiguration("Interface", configName);
        if(loadedComponents.empty()){
            cerr << "WARNING Invalid configuration set for component" << endl; 
            /// @todo Use FATAL function somehow?
            //exit(1);
        }
    }catch(InvalidConfiguration & e){
        cerr << "WARNING Invalid configuration: \"" << configName << "\"" << endl; 
        cerr << e.what() << endl;
        return nullptr;
    }

    // check loaded components and assign them to the right struct elements.
    siox_component * result = new siox_component();
    result->cid.pid = process_data.pid;
    result->uid = uid;

    string instance_str(instance_name);

    if (instance_str.size() > 0){
        result->instance_associate = process_data.association_mapper->create_instance_mapping(instance_str);
    }else{
        result->instance_associate = SIOX_INVALID_ID;
    }

    result->amux = process_data.configurator->searchFor<ActivityMultiplexer>(loadedComponents); 
    if (result->amux == nullptr){
        // link the global AMUX:
        result->amux = process_data.amux;
        assert(result->amux != nullptr);
    }

    return result;
}


void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value){
    assert(attribute != nullptr);
    assert(value != nullptr);
    assert(component != nullptr);

    OntologyValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_component_attribute((component->cid), *attribute, val);
}


siox_component_activity * siox_component_register_activity(siox_unique_interface * uiid, const char * activity_name){
    assert(uiid != SIOX_INVALID_ID);
    assert(activity_name != nullptr);

    return U32_TO_P(process_data.system_information_manager->register_activityID( P_TO_U32(uiid), activity_name));
}


void siox_component_unregister(siox_component * component){
    assert(component != nullptr);
    // Simple implementation: rely on ComponentRegistrar for shutdown.
    delete(component);
}


void siox_report_node_statistics(siox_node * node, siox_attribute * statistic, siox_timestamp start_of_interval, siox_timestamp end_of_interval, void * value){

    // MZ: Das reicht eigentlich bloß an den SMux weiter, oder?
    // Vorher: Statistik zusammenbauen!
    // TODO
}

//############################################################################
/////////////// ACTIVITIES /////////////////////////////
//############################################################################

// MZ: Alle an ActivityBuilder (to be acquired!!!) weiterreichen

// MZ: How do we get our finished activity object back?!
//     In ..._activity_stop()?
// HM: After calling siox_activity_end(), the Activity object (siox_activity * == Activity *) is complete and may be handed over elsewhere.


siox_activity * siox_activity_start(siox_component * component, siox_component_activity * activity){
    assert(component != nullptr);
    assert(activity != nullptr);

    siox_activity* a;
    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    a = ab->startActivity(component->cid, P_TO_U32(activity), nullptr);

    return a;
}


void siox_activity_stop(siox_activity * activity){
    assert(activity != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    ab->stopActivity(activity, nullptr);
}


void siox_activity_set_attribute(siox_activity * activity, siox_attribute * attribute, void * value){
    assert(activity != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();
    Attribute attr(attribute->aID, convert_attribute(attribute, value));

    ab->setActivityAttribute(activity, attr);
}


void siox_activity_report_error(siox_activity * activity, siox_activity_error error){
    assert(activity != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    ab->reportActivityError(activity, error);
}


void siox_activity_end(siox_activity * activity){
    assert(activity != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    ab->endActivity(activity);

    // Send to AMux
    process_data.amux->Log(activity);
}


void siox_activity_link_to_parent(siox_activity * activity_child, siox_activity * activity_parent){
    assert(activity_child != nullptr);
    assert(activity_parent != nullptr);

    ActivityID aid;
    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    aid = activity_parent->aid();
    ab->linkActivities(activity_child, aid);
}

//############################################################################
//////////////// REMOTE CALL ///////////////////////////////////
//############################################################################


siox_remote_call * siox_remote_call_setup(siox_activity * activity, siox_node * target_node, siox_unique_interface * target_unique_interface, siox_associate * target_associate){
    assert(activity != nullptr);

    siox_remote_call* rc;
    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    rc = ab->setupRemoteCall(activity, P_TO_U32(target_node), P_TO_U32(target_unique_interface), P_TO_U32(target_associate));

    return rc;
}


void siox_remote_call_set_attribute(siox_remote_call * remote_call, siox_attribute * attribute, void * value){
    assert(remote_call != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();
    Attribute attr(attribute->aID, convert_attribute(attribute, value));

    ab->setRemoteCallAttribute(remote_call, attr);
}


void siox_remote_call_start(siox_remote_call * remote_call){
    assert(remote_call != nullptr);

    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    ab->startRemoteCall(remote_call, nullptr);
}


siox_activity * siox_activity_start_from_remote_call(siox_component * component, siox_component_activity * activity, siox_node * caller_node, siox_unique_interface * caller_unique_interface, siox_associate * caller_associate){
    assert(component != nullptr);
    assert(activity != nullptr);

    Activity* a;
    ActivityBuilder* ab = ActivityBuilder::getThreadInstance();

    a = ab->startActivity(component->cid, P_TO_U32(activity), P_TO_U32(caller_node), P_TO_U32(caller_unique_interface), P_TO_U32(caller_associate), nullptr);

    return a;
}


//############################################################################
//////////////// ONTOLOGY  ///////////////////////////////////
//############################################################################

siox_attribute * siox_ontology_register_attribute(const char * domain, const char * name, enum siox_ont_storage_type storage_type){
    assert(domain != nullptr);
    assert(name != nullptr);

    // return process_data.ontology->register_attribute(domain, name, convert_attribute_type(storage_type));
    try{
        return & process_data.ontology->register_attribute(domain, name, (VariableDatatype::Type) storage_type);
    }catch(IllegalStateError & e){
        return nullptr;
    }
}


// MZ: TODO change return value to bool, unless this proves C++-incompatible
int siox_ontology_set_meta_attribute(siox_attribute * parent_attribute, siox_attribute * meta_attribute, void * value){
    assert(parent_attribute != nullptr);
    assert(meta_attribute != nullptr);
    assert(value != nullptr);

    AttributeValue val = convert_attribute(meta_attribute, value);
    try{
        process_data.ontology->attribute_set_meta_attribute(*parent_attribute, *meta_attribute, val);
    }catch(IllegalStateError & e){
        return 0;
    }
    return 1;
}


siox_attribute * siox_ontology_register_attribute_with_unit(const char * domain, const char * name, const char * unit, enum siox_ont_storage_type  storage_type){
    assert(domain != nullptr);
    assert(name != nullptr);
    assert(unit != nullptr);

    // MZ: Hier besser statt einer eigenen Domain für Units d verwenden?!
    try{
        const OntologyAttribute & meta = process_data.ontology->register_attribute("Meta", "Unit", VariableDatatype::Type::STRING);
        // OntologyAttribute * attribute = process_data.ontology->register_attribute(d, n, convert_attribute_type(storage_type));
        const OntologyAttribute & attribute = process_data.ontology->register_attribute(domain, name, (VariableDatatype::Type) storage_type);
        process_data.ontology->attribute_set_meta_attribute(attribute, meta, unit);
        return & attribute;
    }catch(IllegalStateError & e){
        return nullptr;
    }    
}


siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name){
    assert(name != nullptr);
    try{
        return & process_data.ontology->lookup_attribute_by_name(domain, name);
    }catch(NotFoundError & e){
        return nullptr;
    }
}


siox_unique_interface * siox_system_information_lookup_interface_id(const char * interface_name, const char * implementation_identifier){
    assert(interface_name != nullptr);
    assert(implementation_identifier != nullptr);

    try{
        return U32_TO_P(process_data.system_information_manager->register_interfaceID(interface_name, implementation_identifier));
    }catch(IllegalStateError & e){
        return 0;
    }
}

} // extern "C"
