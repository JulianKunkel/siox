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

#include <unordered_map>
#include <sstream>
#include <fstream>
#include <pwd.h>
#include <typeinfo>

#include <util/threadSafety.h>

#include <core/module/ModuleLoader.hpp>
#include <core/datatypes/VariableDatatype.hpp>

#include <util/autoLoadModules.hpp>
#include <util/ReporterHelper.hpp>
#include <util/time.h>

#include "siox-ll-internal.hpp"

extern "C" {
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
static bool finalized = true;

/// Number of times SIOX is initalized
static int initalizationCounter = 0;

/// Struct to hold references to global objects needed.
static struct process_info process_data;

static list<void (*)(void)> terminate_cbs;
static list<void (*)(void)> initialization_cbs;
static list<void (*)(void)> terminate_complete_cbs;

static unordered_map<UniqueInterfaceID, siox_component*> registeredComponents;

struct FUNCTION_CLASS{
	FUNCTION_CLASS(){
		monitoring_namespace_inc();
	}

	~FUNCTION_CLASS(){
		monitoring_namespace_dec();
	}
};


struct FUNCTION_PERF_CLASS : FUNCTION_CLASS{
	OverheadEntry * entry;
	Timestamp eventStartTime;

	FUNCTION_PERF_CLASS(OverheadEntry * entry) : entry(entry){		
		eventStartTime = process_data.overhead->startMeasurement();
	}

	~FUNCTION_PERF_CLASS(){
		process_data.overhead->stopMeasurement(entry, eventStartTime);
	}
};

#define PERF_MEASURE_START(name) static OverheadEntry * _oe = process_data.overhead->getOverheadFor(name); FUNCTION_PERF_CLASS _class_inst_x(_oe);
#define FUNCTION_BEGIN static OverheadEntry * _oe = process_data.overhead->getOverheadFor(__FUNCTION__); FUNCTION_PERF_CLASS _class_inst_x(_oe);
#define NO_PERFMEASURE_FUNCTION_BEGIN FUNCTION_CLASS _class_inst_x;

enum SIOX_MONITORING_STATE{
	SIOX_MONITORING_PERMANENTLY_DISABLED = -1,
	SIOX_MONITORING_ENABLED = 0,
	SIOX_MONITORING_DISABLED_BY_USER = 1,
	SIOX_MONITORING_DISABLED_NOT_INITIALIZED = 2
};

static int monitoringDisabled = SIOX_MONITORING_DISABLED_NOT_INITIALIZED;

int siox_is_monitoring_permanently_disabled(){
	return monitoringDisabled == SIOX_MONITORING_PERMANENTLY_DISABLED;
}


int siox_is_monitoring_enabled(){
	return monitoringDisabled == SIOX_MONITORING_ENABLED;
}

void siox_disable_monitoring(){
	if ( monitoringDisabled != SIOX_MONITORING_ENABLED ) return;
	monitoringDisabled = SIOX_MONITORING_DISABLED_BY_USER;
}

void siox_enable_monitoring(){
	if ( monitoringDisabled == SIOX_MONITORING_DISABLED_BY_USER ) return;
	monitoringDisabled = SIOX_MONITORING_ENABLED;
}

int siox_monitoring_namespace_deactivated(){
	return monitoring_namespace_deactivated();
}


//////////////////////////////////////////////////////////

// Functions used for testing the interfaces.
/*ProcessID build_process_id(NodeID hw, uint32_t pid, uint32_t time){
    return {.hw = hw, .pid = pid, .time = time};
}
*/

//////////////////////////////////////////////////////////

NodeID lookup_node_id( const string & hostname )
{
	FUNCTION_BEGIN
	auto ret = process_data.system_information_manager->register_nodeID( hostname );
	
	return ret;
}

void siox_register_termination_signal( void (*func)(void) ){
	for(auto itr = terminate_cbs.begin(); itr != terminate_cbs.end(); itr++ ){
		if ( *itr == func ) return;
	}
	terminate_cbs.push_back(func);
}

void siox_register_termination_complete_signal( void (*func)(void) ){
	for(auto itr = terminate_complete_cbs.begin(); itr != terminate_complete_cbs.end(); itr++ ){
		if ( *itr == func ) return;
	}
	terminate_complete_cbs.push_back(func);	
}

void siox_register_initialization_signal( void (*func)(void) ){
	for(auto itr = initialization_cbs.begin(); itr != initialization_cbs.end(); itr++ ){
		if ( *itr == func ) return;
	}
	initialization_cbs.push_back(func);
}

//////////////////////////////////////////////////////////////////////////////
/// Create a process id object .
//////////////////////////////////////////////////////////////////////////////
/// @param nid [in] The node id of the hardware node the process runs on
//////////////////////////////////////////////////////////////////////////////
/// @return A process id, which is system-wide unique
//////////////////////////////////////////////////////////////////////////////
// Each process can create a runtime ID locally KNOWING the NodeID from the daemon
ProcessID create_process_id( NodeID nid )
{
	pid_t pid = getpid();
	struct timespec tv;

#ifndef __APPLE__
	clock_gettime( CLOCK_MONOTONIC, & tv );
#endif

	// casting will strip off the most significant bits on X86 and hence preserve the current seconds and PID
	ProcessID result = ProcessID();
	result.nid = nid;
	result.pid = ( uint32_t ) pid;
	result.time = ( uint32_t ) tv.tv_sec;
	return result;
}

//////////////////////////////////////////////////////////////////////////////
/// Read a character from a file.
//////////////////////////////////////////////////////////////////////////////
/// @param filename [in]
//////////////////////////////////////////////////////////////////////////////
/// @todo Document this function!
//////////////////////////////////////////////////////////////////////////////
static string readfile( const string & filename )
{
	ifstream file( filename, ios_base::in | ios_base::ate );
	if( ! file.good() ) {
		// @todo add error value.
		return string( "(error in " ) + filename + ")";
	}

	//return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	// Some files from /proc contain 0.
	stringstream s;

	while( ! file.eof() ) {
		char c = 0;
		file >> c;
		if( c == 0 ) {
			c = ' ';
		}
		s << c;
	}

	return s.str();
}

//////////////////////////////////////////////////////////////////////////////
/// Gather relevant process information and attach it as attributes to the
/// current process.
//////////////////////////////////////////////////////////////////////////////
static void add_program_information()
{
	uint64_t pid = getpid();
	uint64_t u64;
	string read;

	OntologyAttribute description;
	description = process_data.ontology->register_attribute( "program", "description/commandLine", VariableDatatype::Type::STRING );

	{
		stringstream s;
		s << "/proc/" << pid << "/cmdline";
		read = readfile( s.str() );
	}
	process_data.association_mapper->set_process_attribute( process_data.pid, description, read );


	description = process_data.ontology->register_attribute( "program", "description/user-id", VariableDatatype::Type::UINT64 );
	u64 = ( uint64_t ) getuid();
	process_data.association_mapper->set_process_attribute( process_data.pid, description, u64 );


	description = process_data.ontology->register_attribute( "program", "description/group-id", VariableDatatype::Type::UINT64 );
	u64 = ( uint64_t ) getgid();
	process_data.association_mapper->set_process_attribute( process_data.pid, description, u64 );

	struct passwd * pwd = getpwuid( getuid() );

	if( pwd != nullptr ) {
		description = process_data.ontology->register_attribute( "program", "description/user-name", VariableDatatype::Type::STRING );
		process_data.association_mapper->set_process_attribute( process_data.pid, description, pwd->pw_name );
	}


	description = process_data.ontology->register_attribute( "program", "description/environment", VariableDatatype::Type::STRING );
	{
		stringstream s;
		s << "/proc/" << pid << "/environ";
		read = readfile( s.str() );
	}
	process_data.association_mapper->set_process_attribute( process_data.pid, description, read );
}


extern "C" {

// Constructor for the shared library

__attribute__( ( constructor ) ) void siox_ctor()
	{
		// for debugging of the LD_PRELOAD wrapper, you may use gdb to attach to the process
		// Inside gdb run: set waiting = 0
		// Then the loop will continue.
		if ( getenv("SIOX_DEBUG_WAIT_LOOP") != nullptr ){
			volatile int waiting = 1;
			while(waiting){
				sleep(1);
			}
		}
		
		NO_PERFMEASURE_FUNCTION_BEGIN
		// Retrieve hostname; NodeID and PID will follow once process_data is set up
		// If necessary, do actual initialisation
		if( finalized ) {
			process_data.overhead = new OverheadStatistics();
			PERF_MEASURE_START("INIT")
			string hostname = util::getHostname();
			try {
				initalizationCounter++;

				// Load required modules and pull the interfaces into global datastructures
				// Use an environment variable and/or configuration files in <DIR> or /etc/siox.conf
				process_data.registrar = new ComponentRegistrar();

				vector<Component *> loadedComponents = util::LoadConfiguration(& process_data.configurator, process_data.registrar);

				// check loaded components and assign them to the right struct elements.
				process_data.ontology = process_data.configurator->searchFor<Ontology>( loadedComponents );
				process_data.system_information_manager = process_data.configurator->searchFor<SystemInformationGlobalIDManager>( loadedComponents );
				process_data.association_mapper =  process_data.configurator->searchFor<AssociationMapper>( loadedComponents );
				process_data.amux = process_data.configurator->searchFor<ActivityMultiplexer>( loadedComponents );
				process_data.optimizer = process_data.configurator->searchFor<knowledge::Optimizer>( loadedComponents );

				assert( process_data.ontology );
				assert( process_data.system_information_manager );
				assert( process_data.association_mapper );
				assert( process_data.amux );

				// Retrieve NodeID and PID now that we have a valid SystemInformationManager
				process_data.nid = lookup_node_id( hostname );
				process_data.pid = create_process_id( process_data.nid );
				process_data.association_mapper->setLocalInformation(hostname, process_data.pid);

				process_data.registrar->start();

			} catch( exception & e ) {
				cerr << "Received exception of type " << typeid( e ).name() << " message: " << e.what() << endl;
				// SIOX will be disabled !
				siox_finalize_monitoring();
				return;
			}

			add_program_information();

			finalized = false;
			monitoringDisabled = SIOX_MONITORING_ENABLED;			
		}		
	}

int siox_initialization_count(){
	return initalizationCounter;
}

void siox_initialize_monitoring(){
	siox_ctor();

	// invoke initialization callbacks
	for( auto itr = initialization_cbs.begin() ; itr != initialization_cbs.end(); itr++ ){
		(*itr)();
	}
}



static void finalizeSIOX(int print){

		if( finalized ) {
			return;
		}

		// invoke terminate callbacks
		for( auto itr = terminate_cbs.begin() ; itr != terminate_cbs.end(); itr++ ){
			(*itr)();
		}

		{			
			PERF_MEASURE_START("FINALIZE")

			process_data.registrar->stopNonMandatoryModules();			

			if( print ){
				OverheadStatisticsDummy * dummyComponent = new OverheadStatisticsDummy( *process_data.overhead );
				process_data.registrar->registerComponent( -1 , "GENERIC", "SIOX_LL", dummyComponent );
				util::invokeAllReporters( process_data.registrar );
			}

			// stop all threads and cleanup memory
			process_data.registrar->stop();
			process_data.registrar->shutdown();
		}

		delete( process_data.registrar );
		delete( process_data.configurator );

		delete( process_data.overhead );

		finalized = true;
		monitoringDisabled = SIOX_MONITORING_PERMANENTLY_DISABLED;
}

// Destructor for the shared library
__attribute__( ( destructor ) ) void siox_ll_dtor()
{
	finalizeSIOX(1);

	// invoke termination complete callbacks
	for( auto itr = terminate_complete_cbs.begin() ; itr != terminate_complete_cbs.end(); itr++ ){
		(*itr)();
	}
}

void siox_finalize_monitoring(){
	monitoringDisabled = SIOX_MONITORING_PERMANENTLY_DISABLED;
	finalizeSIOX(0);
}

void siox_handle_prepare_fork(){
	FUNCTION_BEGIN
	// we have to shutdown all the threads as fork() does not copy them leading to errors of all kind.	
	process_data.registrar->stop();
}

void siox_handle_fork_complete(int im_the_child){
	FUNCTION_BEGIN

	process_data.pid = create_process_id( process_data.nid );
	process_data.association_mapper->setLocalInformation(process_data.association_mapper->localHostname(), process_data.pid);

	// fix the pid() in the existing components
	for(auto itr = registeredComponents.begin(); itr != registeredComponents.end(); itr++ ){
		itr->second->cid.pid.pid = process_data.pid.pid;
	}
	

	// we may re-initialize the child from scratch with new statistics etc.?
	process_data.registrar->start();
}




//############################################################################
///////////////////// Implementation of SIOX-LL /////////////
//############################################################################

/// Variant datatype to uniformly represent the multitude of different attibutes
typedef VariableDatatype AttributeValue;

// we stuff inside the siox_attribute pointer the type (first 32 Bits) and the ID
static OntologyAttribute convertPtrToOntologyAttribute(siox_attribute * ptr){
	return OntologyAttribute((uint32_t)(size_t) ptr, (VariableDatatype::Type) ((size_t)ptr>>32));
}

static siox_attribute * convertOntologyAttributeToPtr(const OntologyAttribute & oa){
	return (void *) (oa.aID + (((uint64_t)oa.storage_type)<<32));
}

//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
	static VariableDatatype convert_attribute( OntologyAttribute & oa, const void * value )
	{
		AttributeValue v;

		switch( oa.storage_type ) {
			case( VariableDatatype::Type::UINT32 ):
				return *( ( uint32_t * ) value );
			case( VariableDatatype::Type::INT32 ): {
				return *( ( int32_t * ) value );
			}
			case( VariableDatatype::Type::UINT64 ):
				return *( ( uint64_t * ) value );
			case( VariableDatatype::Type::INT64 ): {
				return *( ( int64_t * ) value );
			}
			case( VariableDatatype::Type::FLOAT ): {
				return  *( ( float * ) value );
			}
			case( VariableDatatype::Type::DOUBLE ): {
				return  *( ( double * ) value );
			}
			case( VariableDatatype::Type::STRING ): {
				return ( char * ) value;
			}
			case( VariableDatatype::Type::INVALID ): {
				assert( 0 );
			}
		}
		return "";
	}

	static bool convert_attribute_back( OntologyAttribute & oa, const VariableDatatype & val, void * out_value ){
		switch( val.type() ) {
			case VariableDatatype::Type::INT32:
				*((int32_t*) out_value) = val.int32();
				return true;
			case VariableDatatype::Type::UINT32:
				*((uint32_t*) out_value) = val.uint32();
				return true;
			case VariableDatatype::Type::INT64:
				*((int64_t*) out_value) = val.int64();
				return true;
			case VariableDatatype::Type::UINT64:
				*((uint64_t*) out_value) = val.uint64();
				return true;
			case VariableDatatype::Type::FLOAT:
				*((float*) out_value) = val.flt();
				return true;
			case VariableDatatype::Type::DOUBLE:
				*((double*) out_value) = val.dbl();
				return true;
			case VariableDatatype::Type::STRING: {
				*(char**) out_value = strdup(val.str());
				return true;
			}
			case VariableDatatype::Type::INVALID:
			default:
				assert(0 && "tried to optimize for a VariableDatatype of invalid type");
				return false;
		}
	}	


	void siox_process_set_attribute( siox_attribute * attribute, const void * value )
	{
		assert( attribute != nullptr );
		assert( value != nullptr );
		FUNCTION_BEGIN
		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);
		AttributeValue val = convert_attribute( oa, value );
		process_data.association_mapper->set_process_attribute( process_data.pid, oa, val );
	}


	siox_associate * siox_associate_instance( const char * instance_information )
	{
		FUNCTION_BEGIN
		auto ret = U32_TO_P( process_data.association_mapper->create_instance_mapping( instance_information ) );
		return ret;
	}

//############################################################################
/////////////// MONITORING /////////////////////////////
//############################################################################

	siox_node * siox_lookup_node_id( const char * hostname )
	{
		FUNCTION_BEGIN
		if( hostname == NULL ) {
			// we take the localhost's ID.
			return U32_TO_P( process_data.nid );
		}

		auto ret = U32_TO_P( lookup_node_id( hostname ) );
		return ret;
	}

   int siox_component_is_registered( siox_unique_interface * uiid ){
		assert( uiid != SIOX_INVALID_ID );

   	FUNCTION_BEGIN

   	boost::shared_lock<boost::shared_mutex> lock( process_data.critical_mutex );
		UniqueInterfaceID uid = P_TO_U32( uiid );

		return registeredComponents.count(uid) > 0;
   }


	siox_component * siox_component_register( siox_unique_interface * uiid, const char * instance_name )
	{
		assert( uiid != SIOX_INVALID_ID );
		assert( instance_name != nullptr );

		assert( siox_component_is_registered(uiid) == 0 );

		FUNCTION_BEGIN

		boost::upgrade_lock<boost::shared_mutex> lock( process_data.critical_mutex );

		UniqueInterfaceID uid = P_TO_U32( uiid );
		const string & interface_implementation = process_data.system_information_manager->lookup_interface_implementation( uid );
		const string & interface_name = process_data.system_information_manager->lookup_interface_name( uid );

		char configName[1001];
		if( interface_implementation != "" ) {
			snprintf( configName, 1000, "%s %s", interface_name.c_str(), interface_implementation.c_str() );
		} else {
			snprintf( configName, 1000, "%s", interface_name.c_str() );
		}

		cout << "siox_component_register: " << configName << endl;

		vector<Component *> loadedComponents;
		try {
			loadedComponents = process_data.configurator->LoadConfiguration( "Interface", configName );
			if( loadedComponents.empty() ) {
				cerr << "WARNING Invalid configuration set for component" << endl;
				/// @todo Use FATAL function somehow?
				//exit(1);
			}
			process_data.registrar->start();
		} catch( InvalidConfiguration & e ) {
			cerr << "WARNING Invalid configuration: \"" << configName << "\"" << endl;
			cerr << e.what() << endl;
			//return nullptr;
		}

		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock( lock );

		// check loaded components and assign them to the right struct elements.
		siox_component * result = new siox_component();
		assert( result != nullptr );

		result->cid.id = ++process_data.last_componentID;
		result->cid.pid = process_data.pid;
		result->uid = uid;

		registeredComponents[uid] = result;

		string instance_str( instance_name );

		if( instance_str.size() > 0 ) {
			result->instance_associate = process_data.association_mapper->create_instance_mapping( instance_str );
		} else {
			result->instance_associate = SIOX_INVALID_ID;
		}

		result->amux = process_data.configurator->searchFor<ActivityMultiplexer>( loadedComponents );
		if( result->amux == nullptr ) {
			// link the global AMUX:
			result->amux = process_data.amux;
			assert( result->amux != nullptr );
		}

		result->preCallAmux = process_data.configurator->searchFor<ActivityMultiplexerSync>( loadedComponents );

		return result;
	}


	void siox_component_set_attribute( siox_component * component, siox_attribute * attribute, const void * value )
	{
		FUNCTION_BEGIN
		assert( attribute != nullptr );
		assert( value != nullptr );
		assert( component != nullptr );

		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);
		OntologyValue val = convert_attribute( oa, value );		
		process_data.association_mapper->set_component_attribute( ( component->cid ), oa, val );
	}


	siox_component_activity * siox_component_register_activity( siox_unique_interface * uiid, const char * activity_name )
	{
		assert( uiid != SIOX_INVALID_ID );
		assert( activity_name != nullptr );

		FUNCTION_BEGIN
		auto ret = U32_TO_P( process_data.system_information_manager->register_activityID( P_TO_U32( uiid ), activity_name ) );
		return ret;
	}


	void siox_component_unregister( siox_component * component )
	{	
		FUNCTION_BEGIN
		boost::unique_lock<boost::shared_mutex> lock( process_data.critical_mutex );

		if ( ! registeredComponents.count(component->uid) ){
			return;
		}

		registeredComponents.erase(component->uid);

		assert( component != nullptr );

		// Simple implementation: rely on ComponentRegistrar for shutdown.
		delete( component );
	}


	void siox_report_node_statistics( siox_node * node, siox_attribute * statistic, siox_timestamp start_of_interval, siox_timestamp end_of_interval, const void * value )
	{
		FUNCTION_BEGIN

		// MZ: Das reicht eigentlich bloß an den SMux weiter, oder?
		// Vorher: Statistik zusammenbauen!
		// TODO
	}

//############################################################################
/////////////// ACTIVITIES /////////////////////////////
//############################################################################

// MZ: How do we get our finished activity object back?!
//     In ..._activity_stop()?
// HM: After calling siox_activity_end(), the Activity object (siox_activity * == Activity *) is complete and may be handed over elsewhere.


	siox_activity * siox_activity_begin( siox_component * component, siox_component_activity * activity )
	{
		assert( component != nullptr );
		assert( activity != nullptr );

		FUNCTION_BEGIN
		Activity * a;
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();
		//cout << "START: " << ab << endl;

		a = ab->beginActivity( component->cid, P_TO_U32( activity ) );

		return new siox_activity( a, component );
	}

	void siox_activity_start( siox_activity * activity )
	{
		assert( activity != nullptr );

		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();
		// Find component's amux
		siox_component * component = activity->component;
		assert( component != nullptr );

		// Send the activity to it
		if ( ! monitoringDisabled && component->preCallAmux ){
			component->preCallAmux->Log( activity->shrdPtr );
		}
		ab->startActivity( activity->activity, siox_gettime() );
	}


	void siox_activity_stop( siox_activity * activity )
	{
		assert( activity != nullptr );

		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();
		ab->stopActivity( activity->activity, siox_gettime() );
	}


	void siox_activity_set_attribute( siox_activity * activity, siox_attribute * attribute, const void * value )
	{
		assert( activity != nullptr );
		assert( attribute != nullptr );

		if( value == nullptr ) {
			return;
		}

		FUNCTION_BEGIN

		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();
		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);
		Attribute attr( oa.aID, convert_attribute( oa, value ) );

		ab->setActivityAttribute( activity->activity, attr );

	}


	void siox_activity_report_error( siox_activity * activity, siox_activity_error error )
	{
		assert( activity != nullptr );

		FUNCTION_BEGIN

		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		ab->reportActivityError( activity->activity, error );

	}


	void siox_activity_end( siox_activity * activity )
	{
		assert( activity != nullptr );
		assert( activity->activity != nullptr );

		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		//cout << "END: " << ab << endl;

		ab->endActivity( activity->activity );

		// Find component's amux
		siox_component * component = activity->component;
		assert( component != nullptr );

		// Send the activity to it
		if ( ! monitoringDisabled ){ 
			component-> amux->Log( activity->shrdPtr );
		}

		delete( activity );

	}

	siox_activity_ID * siox_activity_get_ID( const siox_activity * activity )
	{
		assert( activity != nullptr );

		assert( sizeof( siox_activity_ID ) == sizeof( ActivityID ) );
		siox_activity_ID * id = ( siox_activity_ID * ) malloc( sizeof( ActivityID ) );
		ActivityID aid = activity->activity->aid();
		memcpy( id, & aid, sizeof( ActivityID ) );
		return id;
	}

	void siox_activity_link_to_parent( siox_activity * activity_child, siox_activity_ID * aid )
	{
		if( aid == nullptr )
			return;

		assert( activity_child != nullptr );

		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		ab->linkActivities( activity_child->activity, *( ( ActivityID * ) aid ) );

	}

//############################################################################
//////////////// REMOTE CALL ///////////////////////////////////
//############################################################################


	siox_remote_call * siox_remote_call_setup( siox_activity * activity, siox_node * target_node, siox_unique_interface * target_unique_interface, siox_associate * target_associate )
	{
		FUNCTION_BEGIN
		assert( activity != nullptr );

		siox_remote_call * rc;
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		rc = ab->setupRemoteCall( activity->activity, P_TO_U32( target_node ), P_TO_U32( target_unique_interface ), P_TO_U32( target_associate ) );

		return rc;
	}


	void siox_remote_call_set_attribute( siox_remote_call * remote_call, siox_attribute * attribute, const void * value )
	{
		assert( remote_call != nullptr );
		assert( attribute != nullptr );
		assert( value != nullptr );
		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);
		Attribute attr( oa.aID, convert_attribute( oa, value ) );

		ab->setRemoteCallAttribute( remote_call, attr );

	}


	void siox_remote_call_start( siox_remote_call * remote_call )
	{
		assert( remote_call != nullptr );
		FUNCTION_BEGIN
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		ab->startRemoteCall( remote_call, nullptr );
	}


	siox_activity * siox_activity_start_from_remote_call( siox_component * component, siox_component_activity * activity, siox_node * caller_node, siox_unique_interface * caller_unique_interface, siox_associate * caller_associate )
	{
		assert( component != nullptr );
		assert( activity != nullptr );

		FUNCTION_BEGIN
		Activity * a;
		ActivityBuilder * ab = ActivityBuilder::getThreadInstance();

		a = ab->beginActivity( component->cid, P_TO_U32( activity ), P_TO_U32( caller_node ), P_TO_U32( caller_unique_interface ), P_TO_U32( caller_associate ) );

		return new siox_activity( a, component );
	}


//############################################################################
//////////////// ONTOLOGY  ///////////////////////////////////
//############################################################################

	siox_attribute * siox_ontology_register_attribute( const char * domain, const char * name, enum siox_ont_storage_type storage_type )
	{
		assert( domain != nullptr );
		assert( name != nullptr );

		FUNCTION_BEGIN
		try {
			OntologyAttribute ret = process_data.ontology->register_attribute( domain, name, ( VariableDatatype::Type ) storage_type );
			return convertOntologyAttributeToPtr(ret);
		} catch( IllegalStateError & e ) {
			return nullptr;
		}
	}


/// @todo change return value to bool, unless this proves C++-incompatible
	int siox_ontology_set_meta_attribute( siox_attribute * parent_attribute, siox_attribute * meta_attribute, const void * value )
	{
		assert( parent_attribute != nullptr );
		assert( meta_attribute != nullptr );
		assert( value != nullptr );

		FUNCTION_BEGIN
		OntologyAttribute oa_meta = convertPtrToOntologyAttribute(meta_attribute);
		OntologyAttribute oa_parent = convertPtrToOntologyAttribute(parent_attribute);

		AttributeValue val = convert_attribute( oa_meta, value );
		try {			
			process_data.ontology->attribute_set_meta_attribute( oa_parent, oa_meta, val );
		} catch( IllegalStateError & e ) {
			return 0;
		}
		return 1;
	}


	siox_attribute * siox_ontology_register_attribute_with_unit( const char * domain, const char * name, const char * unit, enum siox_ont_storage_type  storage_type )
	{
		assert( domain != nullptr );
		assert( name != nullptr );
		assert( unit != nullptr );

		FUNCTION_BEGIN
		try {
			const OntologyAttribute & meta = process_data.ontology->register_attribute( "Meta", "Unit", VariableDatatype::Type::STRING );
			// OntologyAttribute * attribute = process_data.ontology->register_attribute(d, n, convert_attribute_type(storage_type));
			const OntologyAttribute attribute = process_data.ontology->register_attribute( domain, name, ( VariableDatatype::Type ) storage_type );
			process_data.ontology->attribute_set_meta_attribute( attribute, meta, unit );

			return convertOntologyAttributeToPtr(attribute);
		} catch( IllegalStateError & e ) {
			return nullptr;
		}
	}


	siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name )
	{
		FUNCTION_BEGIN
		assert( name != nullptr );
		try {			
			auto ret = process_data.ontology->lookup_attribute_by_name( domain, name );
			return convertOntologyAttributeToPtr(ret);
		} catch( NotFoundError & e ) {
			return nullptr;
		}
	}


	siox_unique_interface * siox_system_information_lookup_interface_id( const char * interface_name, const char * implementation_identifier )
	{
		FUNCTION_BEGIN
		assert( interface_name != nullptr );
		assert( implementation_identifier != nullptr );

		try {
			auto ret = U32_TO_P( process_data.system_information_manager->register_interfaceID( interface_name, implementation_identifier ) );
			return ret;
		} catch( IllegalStateError & e ) {
			return 0;
		}
	}

	int siox_suggest_optimal_value( siox_component * component, siox_attribute * attribute, void * out_value ){
		FUNCTION_BEGIN
		if ( process_data.optimizer == nullptr ){
			return false;
		}		

		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);

		try{
			OntologyValue val(process_data.optimizer->optimalParameter(oa));
			return convert_attribute_back(oa, val, out_value);
		}catch ( NotFoundError & e ){
			return false;
		}		
	}

	int siox_suggest_optimal_value_for( siox_component * component, siox_attribute * attribute, siox_activity * activity, void * out_value ){
		FUNCTION_BEGIN

		if ( process_data.optimizer == nullptr ){
			return false;
		}

		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);

		try{
			OntologyValue val(process_data.optimizer->optimalParameterFor(oa, activity->activity));
			return convert_attribute_back(oa, val, out_value);
		}catch ( NotFoundError & e ){
			return false;
		}		
	}

int siox_suggest_optimal_value_str( siox_component * component, siox_attribute * attribute, char * target_str, int maxLength ){
		if ( process_data.optimizer == nullptr ){
			return 0;
		}

		OntologyAttribute oa = convertPtrToOntologyAttribute(attribute);
		char buff[100];
		sprintf(buff, "siox_suggest_optimal_value_str(%d)", oa.aID);
		string what(buff);
		PERF_MEASURE_START( what.c_str() )

		try{
			OntologyValue val( process_data.optimizer->optimalParameter(oa) );
			strncpy( target_str, val.toStr().c_str(), maxLength );
			return 1;
		}catch ( NotFoundError & e ){
			return 0;
		}
}



} // extern "C"
