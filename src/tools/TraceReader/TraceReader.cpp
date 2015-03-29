#include <assert.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/association_mapper/modules/FileAssociationMapper/FileAssociationMapperOptions.hpp>
#include <monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp>
#include <monitoring/system_information/modules/filebased-system-information/FileBasedSystemInformationOptions.hpp>
#include <monitoring/topology/databaseTopology/DatabaseTopologyOptions.hpp>
#include <monitoring/association_mapper/modules/TopologyAssociationMapper/TopologyAssociationMapperOptions.hpp>
#include <monitoring/ontology/modules/TopologyOntology/TopologyOntologyOptions.hpp>
#include <monitoring/system_information/modules/TopologySystemInformation/TopologySystemInformationOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>
#include <util/time.h>

#include "TraceReader.hpp"

//#include <core/component/ActivitySerializableText.cpp>

std::shared_ptr<Activity> TraceReader::nextActivity() {
	try{
	if( activityDeserializer->hasNextActivity() )
		return activityDeserializer->nextActivity();
	else
		return std::shared_ptr<Activity>{nullptr};
	}catch(exception & e){
		cerr << "Error while reading trace entry: " << e.what() << endl;
		return std::shared_ptr<Activity>{nullptr};
	}
}

TraceReader::TraceReader( string activityFile, string systemInfoFile, string ontologyFile, string associationFile, string topologyDatabase, string activityReader)
{
	if (topologyDatabase == ""){
		a = core::module_create_instance<AssociationMapper>( "", "siox-monitoring-FileAssociationMapper", MONITORING_ASSOCIATION_MAPPER_INTERFACE );
		o = core::module_create_instance<Ontology>( "", "siox-monitoring-FileOntology", ONTOLOGY_INTERFACE );
		s = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-FileBasedSystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );
		FileBasedSystemInformationOptions * sop = new FileBasedSystemInformationOptions();
		sop->filename = systemInfoFile;
		s->init( sop );

		FileOntologyOptions * oop = new FileOntologyOptions();
		oop->filename = ontologyFile;
		o->init( oop );

		FileAssociationMapperOptions * aop = new FileAssociationMapperOptions();
		aop->filename = associationFile;
		a->init( aop );		
	}else{
		t = core::module_create_instance<Topology>( "", "siox-monitoring-DatabaseTopology", MONITORING_TOPOLOGY_INTERFACE );

		// TODO parse me! now HARDCODED
		t->getOptions<DatabaseTopologyOptions>().hostaddress = "10.0.0.202";
		t->getOptions<DatabaseTopologyOptions>().username = "siox";
		t->getOptions<DatabaseTopologyOptions>().password = "siox";
		t->getOptions<DatabaseTopologyOptions>().dbname = "siox";
		t->getOptions<DatabaseTopologyOptions>().port = 5432;
		t->init();
		t->start();

		a = core::module_create_instance<AssociationMapper>( "", "siox-monitoring-TopologyAssociationMapper", MONITORING_ASSOCIATION_MAPPER_INTERFACE );
		a->getOptions<TopologyAssociationMapperOptions>().topology = t;
		a->init();
		a->start();

		o = core::module_create_instance<Ontology>( "", "siox-monitoring-TopologyOntology", ONTOLOGY_INTERFACE );
		o->getOptions<TopologyOntologyOptions>().topology = t;
		o->init();
		o->start();		

		s = core::module_create_instance<SystemInformationGlobalIDManager>( "", "siox-monitoring-TopologySystemInformation", SYSTEMINFORMATION_GLOBALID_MANAGER_INTERFACE );
		s->getOptions<TopologySystemInformationOptions>().topology = t;
		s->init();
		s->start();		
	}

	
	if (activityReader == "") {
		activityDeserializer = core::module_create_instance<ActivitySerializationPlugin>( "", "siox-monitoring-activityPlugin-ActivityBinWriter", ACTIVITY_SERIALIZATION_PLUGIN_INTERFACE );
	}
	else {
		cerr << "Could not find " << activityReader << ". Please check --reader option" << endl;
		exit(1);
	}

	ActivityMultiplexer* amux = core::module_create_instance<ActivityMultiplexer>("", "siox-monitoring-ActivityMultiplexerSync", ACTIVITY_MULTIPLEXER_INTERFACE);
	amux->init();
	amux->start();

	ActivityPluginDereferencing* facade = core::module_create_instance<ActivityPluginDereferencing>("", "siox-knowledge-DereferencingFacade", ACTIVITY_DEREFERENCING_FACADE_INTERFACE);
	DereferencingFacadeOptions& dfo = facade->getOptions<DereferencingFacadeOptions>();
	dfo.topology = nullptr;
	dfo.system_information_manager = s;
	dfo.association_mapper = a;
	dfo.ontology = o;
	dfo.reasoner = nullptr;
	facade->init();
	facade->start();

	assert(amux != nullptr);
	assert(facade != nullptr);

	ActivityMultiplexerPlugin* ke = core::module_create_instance<ActivityMultiplexerPlugin>( "", "siox-monitoring-activityPlugin-KnowledgeExtractor", ACTIVITY_MULTIPLEXER_PLUGIN_INTERFACE);
	ActivityMultiplexerPluginOptions& keo = ke->getOptions<ActivityMultiplexerPluginOptions>();
	keo.multiplexer = amux;
	keo.dereferenceFacade = facade;
	ke->init();
	ke->start();

	assert( activityDeserializer );

	activityDeserializer->loadTrace(activityFile);
	while (true) {
		cout << "pass activity to activity multiplexer" << endl;
		std::shared_ptr<Activity> activity = activityDeserializer->nextActivity();
		if (activity == nullptr) {
			break;
		}
		amux->Log(activity);
	}

	//activityDeserializer = new FileDeserializer<Activity>( activityFile );


	
}

static inline void strtime( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );

	time_t timeIns = ts;
	struct tm * timeStruct = gmtime( & timeIns );

	char buff[200];
	strftime( buff, 199, "%F %T", timeStruct );

	s << buff;
	snprintf( buff, 20, ".%.10lld", ( long long int ) ns );
	s << buff;
}

static inline void strdelta( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );
	char buff[200];
	snprintf( buff, 20, "%lld.%.10lld", ( long long int ) ts, ( long long int ) ns );
	s << buff;
}


void TraceReader::strattribute( const Attribute & a, stringstream & s ) throw( NotFoundError )
{
	OntologyAttributeFull oa = o->lookup_attribute_by_ID( a.id );
	s << oa.domain << "/" << oa.name << "=";
	if( a.value.type() == VariableDatatype::Type::STRING){
		s << '"' << a.value << '"' ;
	}else{
		s << a.value ;
	}
}


void TraceReader::printActivity( std::shared_ptr<Activity> a )
{
	stringstream str;
	try {
		char buff[40];
		siox_time_to_str( a->time_start(), buff, false );

		str << buff << " ";

		strdelta( a->time_stop() - a->time_start(), str );
		if( printHostname )
			str << " " << s->lookup_node_hostname( a->aid().cid.pid.nid );

		str  << " " << a->aid() << " ";

		UniqueInterfaceID uid = s->lookup_interface_of_activity( a->ucaid() );

		str << s->lookup_interface_name( uid ) << " ";
		str << s->lookup_interface_implementation( uid ) << " ";

		str << s->lookup_activity_name( a->ucaid() ) << "(";
		for( auto itr = a->attributeArray().begin() ; itr != a->attributeArray().end(); itr++ ) {
			if( itr != a->attributeArray().begin() ) {
				str << ", ";
			}
			strattribute( *itr, str );
		}
		str << ")";
		str << " = " << a->errorValue();

		if( a->parentArray().begin() != a->parentArray().end() ) {
			str << " ";
			for( auto itr = a->parentArray().begin(); itr != a->parentArray().end(); itr++ ) {
				if( itr != a->parentArray().begin() ) {
					str << ", ";
				}
				str << *itr;
			}
		}

		cout << str.str() << endl;

	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
	}
}


//try{
//   o->set_process_attribute(pid, a1, v2);
//   assert(false);
// }catch(IllegalStateError & e){}

// try{
//  const OntologyValue & vp = o->lookup_process_attribute(pid, a2);
//  assert(false);
// }catch(NotFoundError & e){

// }
