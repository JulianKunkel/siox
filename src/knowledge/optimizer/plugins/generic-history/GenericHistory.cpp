#include <iostream>
#include <list>
#include <unordered_map>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <knowledge/optimizer/OptimizerPluginInterface.hpp>
#include <knowledge/optimizer/Optimizer.hpp>

#include <workarounds.hpp>

#include "GenericHistoryOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;


#define IGNORE_EXCEPTIONS(...) do { try { __VA_ARGS__ } catch(...) { } } while(0)

enum TokenType {
	OPEN = 0,
	ACCESS,
	CLOSE,
	HINT,
	UNKNOWN,
	TOKEN_TYPE_COUNT
};
ADD_ENUM_OPERATORS(TokenType)

/*
 Some thoughts for an abstract plugin.

 This plugin remembers for every program, user, and filename (extensions) efficient and inefficient operations.
 It can be applied to any layer that supports the semantics of OPEN, ACCESS (i.e. read(), write(), etc.), optional HINTS and CLOSE.

 A configuration file defines the symbols which map to the OPEN etc. semantics and an integer descriptor as ontology identifier to track these.
 The time needed for the ACCESS semantics may depend on the abstract attributes: "size", offset (which defines some sort of regularity), and the set of supported HINTS.
 A hint may be set either on OPEN, during ACCESS.
 File extension, user and program are considered to behave similar as a hint.
 
 */
class GenericHistoryPlugin: public ActivityMultiplexerPlugin, public OptimizerInterface {
	public:
		GenericHistoryPlugin() : nTypes{} { }
		void initPlugin() override;
		void initTypes(const shared_ptr<Activity>& activity);
		ComponentOptions * AvailableOptions() override;

		void Notify( shared_ptr<Activity> activity ) override;

		virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) override;

	private:
		Optimizer * optimizer;
		SystemInformationGlobalIDManager * sysinfo;

		string 	interface;
		string 	implementation;
		UniqueInterfaceID 	uiid;

		unordered_map<UniqueComponentActivityID, TokenType>	types;

		int nTypes[TOKEN_TYPE_COUNT];
};



ComponentOptions * GenericHistoryPlugin::AvailableOptions() {
  return new GenericHistoryOptions();
}


void GenericHistoryPlugin::Notify( shared_ptr<Activity> activity ) {
	//cout << "GenericHistoryPlugin received " << activity << endl;

	TokenType type = UNKNOWN;
	IGNORE_EXCEPTIONS( type = types.at( activity->ucaid() ); );

	nTypes[type]++;

	cout << "Generic History saw activity of type " << activity->ucaid() << " => ";
	for(size_t i = 0; i < TOKEN_TYPE_COUNT; i++) cerr << nTypes[i] << " ";
	cerr << "\n";

	switch (type) {
		case OPEN: break;
		case ACCESS: break;
		case CLOSE: optimalParameter( *(OntologyAttribute*)NULL ); break;
		case HINT: break;
		default: break;
	}
}


void GenericHistoryPlugin::initPlugin() {
	fprintf(stderr, "GenericHistoryPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);

	// Retrieve options
	GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();

	// Retrieve pointers to required modules
	assert(optimizer = GET_INSTANCE(Optimizer, o.optimizer));
	assert(sysinfo = facade->get_system_information());

	// Retrieve interface, implementation and uiid
	interface = o.interface;
	implementation = o.implementation;
	try{
		uiid = sysinfo->lookup_interfaceID(interface, implementation);
	}
	catch(NotFoundError)
	{
		cerr << "No UniqueInterfaceID for Interface \"" << interface << "\" and implementation \"" << implementation << "\" found - aborting!" << endl;
		abort();
	}

	// Fill token map with known activities' UCAIDs and their type
	try{
		for( auto itr = o.openTokens.begin(); itr != o.openTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = OPEN;

		for( auto itr = o.accessTokens.begin(); itr != o.accessTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = ACCESS;

		for( auto itr = o.closeTokens.begin(); itr != o.closeTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = CLOSE;

		for( auto itr = o.hintTokens.begin(); itr != o.hintTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = HINT;
	}
	catch(NotFoundError)
	{
		cerr << "No UniqueComponentActivityID for one or more activities of Interface \"" << interface << "\" and implementation \"" << implementation << "\" found - aborting!" << endl;
		abort();
	}
}


OntologyValue GenericHistoryPlugin::optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) {

	cout << "Up to now, GenericHistoryPlugin saw\n";
	cout << "\t" << nTypes[OPEN] << " OPENs\n";
	cout << "\t" << nTypes[ACCESS] << " ACCESSes\n";
	cout << "\t" << nTypes[CLOSE] << " CLOSEs\n";
	cout << "\t" << nTypes[HINT] << " HINTs\n";

	return OntologyValue( 42 );
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new GenericHistoryPlugin();
	}
}
