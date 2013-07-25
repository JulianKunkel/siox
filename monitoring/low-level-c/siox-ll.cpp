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

NodeID lookup_node_id(string & hostname){
    return process_data.system_information_manager->node_id(hostname);
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

//############################################################################
///////////////////// Implementation of SIOX-LL /////////////
//############################################################################

/// Variant datatype to uniformly represent the multitude of different attibutes
typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> AttributeValue;

//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
static AttributeValue convert_attribute(siox_attribute * attribute, void * value){
    AttributeValue v;
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
    assert(attribute != nullptr);
    assert(value != nullptr);

    AttributeValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_process_attribute( & process_data.pid, attribute, val);
}


siox_associate * siox_associate_instance(const char * instance_information){
    string instance(instance_information);
    uint64_t id = process_data.association_mapper->create_instance_mapping(instance);
    // Be aware that this cast is dangerous. For future extensionability this can be replaced with a struct etc.
    return (AssociateID*) id;
}

//############################################################################
/////////////// MONITORING /////////////////////////////
//############################################################################

siox_component * siox_component_register(UniqueInterfaceID * uiid, const char * instance_name){
    assert(uiid != nullptr);
    // instance_name could be nullptr

    ComponentID * result = new ComponentID();
    result->pid = process_data.pid;

    // MZ: TODO Hier eigentlich:
    // Autoconfigurator konfiguriert Schicht, (MZ: Wie ruft man das auf?!?)
    // AMux wird erstellt und zugewiesen, (MZ: Wie ruft man das auf?!?)
    // ABuilder wird erstellt und zugewiesen, (MZ: Wie ruft man das auf?!?)
    // ...
    // MZ: How about the SMux?!

    return result;
}


void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value){
    assert(attribute != nullptr);
    assert(value != nullptr);

    // MZ: TODO siox_component in ComponentID wandeln
    ComponentID * cid = nullptr; // FIXME
    AttributeValue val = convert_attribute(attribute, value);
    process_data.association_mapper->set_component_attribute(cid, attribute, val);
}


siox_component_activity * siox_component_register_activity(siox_unique_interface * uiid, const char * activity_name){
    assert(uiid != nullptr);
    assert(activity_name != nullptr);

    string n(activity_name);
    uint64_t id = process_data.system_information_manager->activity_id(*uiid, n);
    // Be aware that this cast is dangerous. For future extensionability this can be replaced with a struct etc.
    return (UniqueComponentActivityID*) id;
}


void siox_component_unregister(siox_component * component){
    // MZ: Hat noch keine Zielfunktion!
    // TODO

    // Plugins runterfahren,
    // Speicher freigeben,
    // ...
    // IM MOMENT NICHT ZU IMPLEMENTIEREN!!!
}


void siox_report_node_statistics(siox_node node, siox_attribute * statistic, siox_timestamp start_of_interval, siox_timestamp end_of_interval, void * value){

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


siox_activity * siox_activity_start(siox_component_activity * activity){
    assert(activity != nullptr);

    // MZ: return process_data.activity_builder->...;
    return nullptr;
}


void siox_activity_stop(siox_activity * activity){
    assert(activity != nullptr);

    // process_data.activity_builder->stopActivity(activity);
}


void siox_activity_set_attribute(siox_activity * activity, siox_attribute * attribute, void * value){
    assert(activity != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    AttributeValue val = convert_attribute(attribute, value);
    // MZ: TODO Signatur anpassen!
    // process_data.activity_builder->addActivityAttribute(activity, attribute, val);
}


void siox_activity_report_error(siox_activity * activity, int64_t error){
    assert(activity != nullptr);

    // process_data.activity_builder->reportActivityError(activity, error);
}


void siox_activity_end(siox_activity * activity){
    assert(activity != nullptr);

    // process_data.activity_builder->endActivity(activity);

    // TODO Fertige Activity von activity_builder abfordern und an AMux (objekt?!?)
    //     weiterreichen?
}


void siox_activity_link_to_parent(siox_activity * activity_child, siox_activity * activity_parent){
    // TODO
}

//############################################################################
//////////////// REMOTE CALL ///////////////////////////////////
//############################################################################


siox_remote_call * siox_remote_call_start(siox_activity         * activity,
                                          siox_node             * target_nid,
                                          siox_unique_interface * target_uid,
                                          siox_associate        * target_iid){
    // TODO
    return nullptr;
}


void siox_remote_call_set_attribute(siox_remote_call * remote_call, siox_attribute * attribute, void * value){

    // TODO
}


void siox_remote_call_submitted(siox_remote_call * remote_call){

    // TODO
}


void siox_activity_started_by_remote_call(siox_activity * activity, NodeID * caller_NodeID_if_known, UniqueInterfaceID * caller_uid_if_known, AssociateID * caller_instance_if_known){

    // TODO
}


//############################################################################
//////////////// ONTOLOGY  ///////////////////////////////////
//############################################################################

siox_attribute * siox_ontology_register_attribute(const char * domain, const char * name, enum siox_ont_storage_type storage_type){
    assert(domain != nullptr);
    assert(name != nullptr);

    string d(domain);
    string n(name);
    return process_data.ontology->register_attribute(d, n, storage_type);
}


// MZ: TODO change return value to bool, unless this proves C++-incompatible
int siox_ontology_set_meta_attribute(siox_attribute * parent_attribute, siox_attribute * meta_attribute, void * value){
    assert(parent_attribute != nullptr);
    assert(meta_attribute != nullptr);
    assert(value != nullptr);

    AttributeValue val = convert_attribute(meta_attribute, value);
    return process_data.ontology->attribute_set_meta_attribute(parent_attribute, meta_attribute, val);
}


siox_attribute * siox_ontology_register_attribute_with_unit(const char * domain, const char * name, const char * unit, enum siox_ont_storage_type  storage_type){
    assert(domain != nullptr);
    assert(name != nullptr);
    assert(unit != nullptr);

    string d(domain);
    string n(name);
    string ud("Units");
    string u(unit);

    // MZ: Hier besser statt einer eigenen Domain für Units d verwenden?!
    OntologyAttribute * meta = process_data.ontology->register_attribute(ud, u, SIOX_STORAGE_STRING);
    OntologyAttribute * attribute = process_data.ontology->register_attribute(d, n, storage_type);
    process_data.ontology->attribute_set_meta_attribute(attribute, meta, u);

    return attribute;
}


siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name){
    assert(name != nullptr);

    string d(domain);
    string n(name);
    return process_data.ontology->lookup_attribute_by_name(d, n);
}


siox_unique_interface * siox_system_information_lookup_interface_id(const char * interface_name, const char * implementation_identifier){
    assert(interface_name != nullptr);
    assert(implementation_identifier != nullptr);

    string in(interface_name);
    string ii(implementation_identifier);
    //uint64_t id = process_data.system_information_manager->interface_id(in, ii);
    // Be aware that this cast is dangerous. For future extensionability this can be replaced with a struct etc.
    //return (UniqueInterfaceID_*) id;
    UniqueInterfaceID * result = new UniqueInterfaceID(process_data.system_information_manager->interface_id(in, ii));
    return result;
}

} // extern "C"
