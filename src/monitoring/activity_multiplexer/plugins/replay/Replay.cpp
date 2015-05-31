#include <regex>
#include <unordered_map>
#include <iostream>
#include <string>

#include <util/time.h>
#include <monitoring/ontology/Ontology.hpp>
#include "Replay.hpp"


ComponentOptions* ReplayPlugin::AvailableOptions()
{
	return new ReplayPluginOptions{};
}

void ReplayPlugin::initPlugin(){
	assert(multiplexer);
	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&ReplayPlugin::notify), false);
	sys_info = facade->get_system_information();
	assert(sys_info);
	std::cout << "Time (HH:MM:SS.NANO)\tDuration\tID\tComponent\tActivity(Attributes)\tParents = ReturnCode" << std::endl;
}

void ReplayPlugin::notify(const std::shared_ptr<Activity>& activity, int lost){
	ReplayPluginOptions& opts = getOptions<ReplayPluginOptions>();
	switch (opts.verbosity) {
		case 0:
			replayActivity( activity );
			break;
		case 1:
			activity->print();
			break;
		default:
			cout << "Warninig: invalid verbosity level: " << opts.verbosity << endl;
			replayActivity( activity );
	}
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


void ReplayPlugin::strattribute( const Attribute& attribute, std::stringstream& s ) throw( NotFoundError )
{
	OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
	s << oa.domain << "/" << oa.name << "=";
	if( attribute.value.type() == VariableDatatype::Type::STRING){
		s << '"' << attribute.value << '"' ;
	}else{
		s << attribute.value ;
	}
}


void ReplayPlugin::replayActivity( std::shared_ptr<Activity> activity )
{
	stringstream str;
	try {
		char buff[40];
		siox_time_to_str( activity->time_start(), buff, false );


		str << "REPLAY:" << " ";

		str << buff << " ";

		strdelta( activity->time_stop() - activity->time_start(), str );
//		if( printHostname )
//			str << " " << sys_info->lookup_node_hostname( a->aid().cid.pid.nid );

		str  << " " << activity->aid() << " ";

		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( activity->ucaid() );

		str << sys_info->lookup_interface_name( uid ) << " ";
		str << sys_info->lookup_interface_implementation( uid ) << " ";

		str << sys_info->lookup_activity_name( activity->ucaid() ) << "(";
		for( auto itr = activity->attributeArray().begin() ; itr != activity->attributeArray().end(); itr++ ) {
			if( itr != activity->attributeArray().begin() ) {
				str << ", ";
			}
			strattribute( *itr, str );
		}
		str << ")";
		str << " = " << activity->errorValue();

		if( activity->parentArray().begin() != activity->parentArray().end() ) {
			str << " ";
			for( auto itr = activity->parentArray().begin(); itr != activity->parentArray().end(); itr++ ) {
				if( itr != activity->parentArray().begin() ) {
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

extern "C" {
void* MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME ()
	{
		return new ReplayPlugin();
	}
}
