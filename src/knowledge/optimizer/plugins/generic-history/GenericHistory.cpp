#include <iostream>
#include <list>
#include <unordered_map>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>
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

class HintPerformance {
	vector<Attribute> hints;
	int measurementCount;
	double averagePerformance;
};

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

		string interface;
		string implementation;
		UniqueInterfaceID uiid;

		// Map ucaid to type of activity (Open/Access/Close/Hint)
		unordered_map<UniqueComponentActivityID, TokenType> types;
		// Map ucaid to ID of attribute used in computing activity performance
		unordered_map<UniqueComponentActivityID, OntologyAttributeID> performanceAttIDs;

		// Array holding the attribute descriptions of attributes to be used as hints
		vector<OntologyAttribute> hintAttributes;

		int nTypes[TOKEN_TYPE_COUNT];

		unordered_map<ActivityID, vector<Attribute> > openFileHints;
//		unordered_map<string, vector<HintPerformance> > userHints;
//		unordered_map<string, vector<HintPerformance> > appHints;
//		unordered_map<pair<string, string>, vector<HintPerformance> > userAppHints;
		vector<HintPerformance> hints;

		double recordPerformance( const shared_ptr<Activity>& activity );
		vector<Attribute>* findCurrentHints( const shared_ptr<Activity>& activity, ActivityID* outParentId );	//outParentId may be NULL
		static void rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity );
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
	cout << "\n";

	switch (type) {

		case OPEN:
			rememberHints( &openFileHints[activity->aid()], activity );
			break;

		case ACCESS:
			cout << "\t(Performance: " << recordPerformance(activity) << ")" << endl;
			break;

		case CLOSE: {
			ActivityID openFileHintsKey;
			if( findCurrentHints( activity, &openFileHintsKey ) ) {
				openFileHints.erase( openFileHintsKey );
			}
			optimalParameter( *(OntologyAttribute*)NULL );
			break;
		}

		case HINT:
			if( vector<Attribute>* curHints = findCurrentHints( activity, 0 ) ) {
				rememberHints( curHints, activity );
			}
			break;

		default:
			break;
	}
}


void GenericHistoryPlugin::initPlugin() {
	fprintf(stderr, "GenericHistoryPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);

	// Retrieve options
	GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();

	// Retrieve pointers to required modules
	optimizer = GET_INSTANCE(Optimizer, o.optimizer);
	sysinfo = facade->get_system_information();
	assert(optimizer && sysinfo);

	// Retrieve interface, implementation and uiid
	interface = o.interface;
	implementation = o.implementation;
	try{
		uiid = sysinfo->lookup_interfaceID(interface, implementation);
	}
	catch(NotFoundError) {
		cerr << "No UniqueInterfaceID for Interface \"" << interface << "\" and implementation \"" << implementation << "\" found - aborting!" << endl;
		abort();
	}

	// Fill token maps with every known activity's UCAID, type and perfomance-relevant attribute's OAID
	try{
		for( auto itr = o.openTokens.begin(); itr != o.openTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = OPEN;

		// Special care has to be taken that both arrays used are of identical size
		assert(o.accessTokens.size() == o.accessRelevantOntologyAttributes.size());
		auto itr = o.accessTokens.begin();
		auto atr = o.accessRelevantOntologyAttributes.begin();
		while (itr != o.accessTokens.end()) {
			UniqueComponentActivityID ucaid = sysinfo->lookup_activityID(uiid,*itr);
			types[ucaid] = ACCESS;
			performanceAttIDs[ucaid] = facade->lookup_attribute_by_name(atr->first, atr->second).aID;
			itr++;
			atr++;
		}

		for( auto itr = o.closeTokens.begin(); itr != o.closeTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = CLOSE;

		for( auto itr = o.hintTokens.begin(); itr != o.hintTokens.end(); itr++ )
			types[sysinfo->lookup_activityID(uiid,*itr)] = HINT;
	}
	catch(NotFoundError) {
		cerr << "No UniqueComponentActivityID for one or more activities of interface \"" << interface << "\" and implementation \"" << implementation << "\" found - aborting!" << endl;
		abort();
	}

	// Find Attribute descriptions for attributes used as hints and remember them
	for( auto itr = o.hintAttributes.begin(); itr != o.hintAttributes.end(); itr++ )
	{
		string domain = itr->first;
		string attribute = itr->second;

		try{
			OntologyAttribute ontatt = facade->lookup_attribute_by_name(domain, attribute);
			hintAttributes.push_back(ontatt);
		}
		catch(NotFoundError)
		{
			cerr << "No OntologyAttribute for interface \"" << domain << "\" and attribute \"" << attribute << "\" found - aborting!" << endl;
			abort();
		}

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


double GenericHistoryPlugin::recordPerformance( const shared_ptr<Activity>& activity ){
	Timestamp t_start = activity->time_start();
	Timestamp t_stop = activity->time_stop();
	OntologyAttributeID oaid = 0;	//According to datatypes/ids.hpp this is an illegal value. I hope that information is still up to date.

	cout << t_start << "-" << t_stop << "@";

	if (t_stop == t_start) return 1/0.0;	//Infinite performance is sematically closer to an almost zero time operation than zero performance.

	// Find performance-relevant attribute's value for activity
	IGNORE_EXCEPTIONS(oaid = performanceAttIDs.at(activity->ucaid()););
	const vector<Attribute> & attributes = activity->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if (itr->id == oaid) {
			uint64_t value = itr->value.uint64();
			cout << value << " => ";
			return ((double) value) / (t_stop - t_start) ;
		}
	}

	return 0/0.0;	//return a NAN if the attribute was not found
}


vector<Attribute>* GenericHistoryPlugin::findCurrentHints( const shared_ptr<Activity>& activity, ActivityID* outParentId ) {
	const vector<ActivityID>& parents = activity->parentArray();
	for( size_t i = parents.size(); i--; ) {
		vector<Attribute>* savedHints = 0;
		IGNORE_EXCEPTIONS( savedHints = &openFileHints.at( parents[i] ); );
		if( savedHints ) {
			if(outParentId) *outParentId = parents[i];
			return savedHints;
		}
	}
	return 0;
}


void GenericHistoryPlugin::rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity ) {
	outHintVector->clear();
	const vector<Attribute>& attributes = activity.attributeArray();
	for( size_t i = attributes->size(); i--; ) {
		bool isHint = false;
		IGNORE_EXCEPTIONS( hintTypes.at( attributes[i].id ); isHint = true; );
		if( isHint ) outHintVector->push_back( attributes[i] );
	}
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new GenericHistoryPlugin();
	}
}
