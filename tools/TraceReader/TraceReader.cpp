#include <assert.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include <core/module/ModuleLoader.hpp>

#include <monitoring/association_mapper/modules/FileAssociationMapper/FileAssociationMapperOptions.hpp>
#include <monitoring/ontology/modules/file-ontology/FileOntologyOptions.hpp>
#include <monitoring/system_information/modules/filebased-system-information/FileBasedSystemInformationOptions.hpp>

#include "TraceReader.hpp"

#include <build/core/component/ActivitySerializableText.hpp>

Activity * TraceReader::nextActivity() {
	if( activityDeserializer->hasNext() )
		return activityDeserializer->parseNext();
	else
		return nullptr;
}

TraceReader::TraceReader( string activityFile, string systemInfoFile, string ontologyFile, string associationFile )
{
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
	activityDeserializer = new FileDeserializer<Activity>( activityFile );
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
	const OntologyAttribute & oa = o->lookup_attribute_by_ID( a.id );
	s << oa.domain << "/" << oa.name << "=";
	if( a.value.type() == VariableDatatype::Type::STRING){
		s << '"' << a.value << '"' ;
	}else{
		s << a.value ;
	}
}


void TraceReader::printActivity( Activity * a )
{

	stringstream str;
	try {
		
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