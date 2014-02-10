#include <iostream>
#include <sstream>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <mutex>

#include <util/ExceptionHandling.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
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


const int kMinTrialCount = 5;

#if 0
	class FunctionCallTracker {
		public:
			FunctionCallTracker(const char* functionName) : functionName(functionName){
				cerr << "[GenericHistoryPlugin]: " << functionName << "\n";
			}
			~FunctionCallTracker() {
				cerr << "[GenericHistoryPlugin]: leaving " << functionName << "\n";
			}
		private:
			const char* functionName;
	};
	#define TRACK_FUNCTION_CALLS FunctionCallTracker functionCallTrackerInstance(__PRETTY_FUNCTION__);
#else
	#define TRACK_FUNCTION_CALLS
#endif

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
	public:
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

 XXX: Should work if the same file is opened several times, but the code is not thread safe.
 
 */
/// @todo TODO: This code smells; we have 13 data members and an initPlugin() of 83 lines. Redesign.
class GenericHistoryPlugin: public ActivityMultiplexerPlugin, public OptimizerInterface, public ComponentReportInterface {
	public:
		GenericHistoryPlugin() : nTypes{} { }
		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		void Notify( const shared_ptr<Activity> & activity, int lost );

		OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) override;

		OntologyValue optimalParameterFor( const OntologyAttribute & attribute, const Activity * activityToStart ) const throw( NotFoundError ) override{
			return optimalParameter(attribute);
		}

		ComponentReport prepareReport() override;
		void finalize() override;
		~GenericHistoryPlugin();

	private:
		bool tryEnsureInitialization();	//If we are not initialized yet, try to do it. Returns true if initialization could be ensured.
		double recordPerformance( const shared_ptr<Activity>& activity );
		vector<Attribute>* findCurrentHints( const shared_ptr<Activity>& activity, ActivityID* outParentId );	//outParentId may be NULL
		void rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity );

		///////////////////////////

		mutex giant_mutex;

		int initLevel = 0;
		const int initializedLevel = -1;

		UniqueInterfaceID uiid;
		// AttID of the attribute user-id
		OntologyAttributeID uidAttID;	///@todo TODO: This variable is currently dead code. Remove it or use it.

		// maps to quickly lookup the type (open/access/close/hint), ontology attribute ID and datatype of an activity
		unordered_map<UniqueComponentActivityID, TokenType> types;
		unordered_map<UniqueComponentActivityID, OntologyAttributeID> performanceAttIDs;
		unordered_map<OntologyAttributeID,VariableDatatype::Type> hintTypes;

		// Number of activities with the respective token type seen up to now
		int nTypes[TOKEN_TYPE_COUNT];	///@todo TODO: This variable is currently dead code. Remove it or use it.
		TokenType lastActionType = UNKNOWN;

		unordered_map<ActivityID, vector<Attribute> > openFileHints;
//		unordered_map<string, vector<HintPerformance> > userHints;
//		unordered_map<string, vector<HintPerformance> > appHints;
//		unordered_map<pair<string, string>, vector<HintPerformance> > userAppHints;
		vector<HintPerformance> performanceHistory;
};



ComponentOptions * GenericHistoryPlugin::AvailableOptions() {
	TRACK_FUNCTION_CALLS
	return new GenericHistoryOptions();
}

void GenericHistoryPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	TRACK_FUNCTION_CALLS
	//cout <<"[GenericHistory]: " << "received " << activity << endl;
	if( ! tryEnsureInitialization() ) return;

	TokenType type = UNKNOWN;
	
	unique_lock<mutex> lock( giant_mutex );

	IGNORE_EXCEPTIONS( type = types.at( activity->ucaid() ); );

	nTypes[type]++;

//	cerr <<"[GenericHistory]: " << "saw activity of type " << activity->ucaid() << " => ";
//	for(size_t i = 0; i < TOKEN_TYPE_COUNT; i++) cerr << nTypes[i] << " ";
//	cerr << "\n";


	switch (type) {

		case OPEN:
//			cerr <<"[GenericHistory]: " << "openFileHints[" << activity->aid() << "]\n";
			rememberHints( &openFileHints[activity->aid()], activity );
			break;

		case ACCESS: {
			vector<Attribute>* curHints = findCurrentHints( activity, 0 );
			if( curHints ) {
				double curPerformance = recordPerformance( activity );
				bool foundHints = false;
				for( size_t i = performanceHistory.size(); i--; ) {
					HintPerformance& temp = performanceHistory[i];
					if( temp.hints == *curHints ) {
						temp.averagePerformance = ( temp.averagePerformance*( temp.measurementCount ) + curPerformance )/( temp.measurementCount + 1 );
						temp.measurementCount++;
						foundHints = true;
						break;
					}
				}
				if( !foundHints ) {
					performanceHistory.emplace_back((HintPerformance){
						.hints = *curHints,
						.measurementCount = 1,
						.averagePerformance = curPerformance
					});
				}
			}
			break;
		}

		case CLOSE: {
			ActivityID openFileHintsKey;
			if( findCurrentHints( activity, &openFileHintsKey ) ) {
				openFileHints.erase( openFileHintsKey );
			}
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
	lastActionType = type;
}


void GenericHistoryPlugin::initPlugin() {
	TRACK_FUNCTION_CALLS

	// Retrieve options
	GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();
	SystemInformationGlobalIDManager * sysinfo = facade->get_system_information();
	assert(sysinfo);

	switch(initLevel) {
		case 0:
			initLevel = 1;	//Once we are called from outside, we must have sensible options, so if initialization fails now, we can simply retry.

		case 1:
			// Retrieve uiid
			RETURN_ON_EXCEPTION( uiid = sysinfo->lookup_interfaceID(o.interface, o.implementation); );
			initLevel = 2;

		case 2:
			// Fill token maps with every known activity's UCAID, type and perfomance-relevant attribute's OAID
			RETURN_ON_EXCEPTION(
				for( auto itr = o.openTokens.begin(); itr != o.openTokens.end(); itr++ )
					types[sysinfo->lookup_activityID(uiid,*itr)] = OPEN;
			);
			initLevel = 3;

		case 3:
			// Special care has to be taken that both arrays used are of identical size
			assert(o.accessTokens.size() == o.accessRelevantOntologyAttributes.size());
			RETURN_ON_EXCEPTION(
				auto itr = o.accessTokens.begin();
				auto atr = o.accessRelevantOntologyAttributes.begin();
				while (itr != o.accessTokens.end()) {
					UniqueComponentActivityID ucaid = sysinfo->lookup_activityID(uiid,*itr);
					types[ucaid] = ACCESS;
					performanceAttIDs[ucaid] = facade->lookup_attribute_by_name(atr->first, atr->second).aID;
					itr++;
					atr++;
				}
			);
			initLevel = 4;

		case 4:
			RETURN_ON_EXCEPTION(
				for( auto itr = o.closeTokens.begin(); itr != o.closeTokens.end(); itr++ )
					types[sysinfo->lookup_activityID(uiid,*itr)] = CLOSE;
			);
			initLevel = 5;

		case 5:
			RETURN_ON_EXCEPTION(
				for( auto itr = o.hintTokens.begin(); itr != o.hintTokens.end(); itr++ )
					types[sysinfo->lookup_activityID(uiid,*itr)] = HINT;
			);
			initLevel = 6;

		case 6:
			// Find oaids and value types for attributes used as hints and remember them
			RETURN_ON_EXCEPTION(
				for( auto itr = o.hintAttributes.begin(); itr != o.hintAttributes.end(); itr++ ) {
					string domain = itr->first;
					string attribute = itr->second;

					OntologyAttribute ontatt = facade->lookup_attribute_by_name(domain, attribute);
					hintTypes[ontatt.aID]=ontatt.storage_type;
				}
			);
			initLevel = 7;

		case 7:
			// Walk through the attributes again and register this as an optimizer for them.
			// This is done in a separate step, because the previous loop may exit with an exception,
			// and the optimizer does not tollerate us registering ourselves twice for the same attribute.
			// This time we should not get any exceptions since we were already able to lookup the attributes once.
			try{
				Optimizer * optimizer = GET_INSTANCE(Optimizer, o.optimizer);
				assert(optimizer);
				for( auto itr = o.hintAttributes.begin(); itr != o.hintAttributes.end(); itr++ ) {
					string domain = itr->first;
					string attribute = itr->second;

					OntologyAttribute ontatt = facade->lookup_attribute_by_name(domain, attribute);
					optimizer->registerPlugin( ontatt, this );
				}
			} catch(...) {
				assert(0 && "Fatal error, cannot look up an attribute that could be looked up previously. Please report this bug."), abort();
			}
			initLevel = 8;

		case 8:
			// Find and remember various other OAIDs
			RETURN_ON_EXCEPTION( uidAttID = facade->lookup_attribute_by_name("program","description/user-id").aID; );
			multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( &GenericHistoryPlugin::Notify ), false );
			initLevel = 9;
	}
	initLevel = initializedLevel;
}


GenericHistoryPlugin::~GenericHistoryPlugin() {
	TRACK_FUNCTION_CALLS
	GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();
	Optimizer * optimizer = GET_INSTANCE(Optimizer, o.optimizer);
	// Unregister all attributes we claimed as optimizable with the optimizer
	for( auto itr = hintTypes.begin(); itr != hintTypes.end(); itr++ ) {
		OntologyAttribute ontatt;
		try {
			ontatt = facade->lookup_attribute_by_ID(itr->first);
		}
		catch( NotFoundError ) {
			cerr << "[GenericHistory]: " << "Could not find attribute # \"" << itr->first << "\" - continuing." << endl;
			continue; // As we're closing down at any rate, this is not too dramatic
		}
		optimizer->unregisterPlugin( ontatt );	// Tell the optimizer not to ask us for this attribute any more
	}
}


OntologyValue GenericHistoryPlugin::optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) {
	TRACK_FUNCTION_CALLS
	if( initLevel != initializedLevel ) throw( NotFoundError() );	// Need to be initialized
	if( lastActionType == HINT ) throw( NotFoundError() );	// The user is king => don't override his choices
	if( !performanceHistory.size() ) throw( NotFoundError() );	// No past experience => nothing to suggest

	// Retrieve the hintset with the best performance and with the least measurements from the history.
	const HintPerformance* leastMeasurements = &performanceHistory[0];
	const HintPerformance* maxPerformance = &performanceHistory[0];
	for( size_t i = performanceHistory.size(); i --> 1; ) {
		if( performanceHistory[i].measurementCount < leastMeasurements->measurementCount ) leastMeasurements = &performanceHistory[i];
		if( performanceHistory[i].averagePerformance > maxPerformance->averagePerformance ) maxPerformance = &performanceHistory[i];
	}
	// Choose which hintset to use.
	const HintPerformance* result = maxPerformance;
	if( leastMeasurements->measurementCount < kMinTrialCount ) result = leastMeasurements;
	// Find the parameter for the requested hint.
	if( result && result->hints.size() ) {
		for( size_t i = result->hints.size(); i--; ) {
			if( result->hints[i].id == attribute.aID) return result->hints[i].value;
		}
	}
	throw( NotFoundError() );
}


ComponentReport GenericHistoryPlugin::prepareReport() {
	TRACK_FUNCTION_CALLS
	if( !tryEnsureInitialization() ) return ComponentReport();
	ostringstream reportText;
	reportText << "{\n";
	for( size_t i = performanceHistory.size(); i--; ) {
		HintPerformance& curStatistic = performanceHistory[i];
		reportText << "\tavrg performance = " << curStatistic.averagePerformance << " (" << curStatistic.measurementCount << " measurements), hints = { ";
		for( size_t j = curStatistic.hints.size(); j--; ) {
			Attribute& curHint = curStatistic.hints[j];
			reportText << "(" << curHint.id << ", " << curHint.value << ((j) ? "), " : ")");
		}
		reportText << "}\n";
	}
	reportText << "}\n";
	ComponentReport result;

	result.addEntry("Hint Statistics", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_CRITICAL, VariableDatatype( reportText.str() ) ));
	return result;
}


bool GenericHistoryPlugin::tryEnsureInitialization() {
	unique_lock<mutex> lock( giant_mutex );

	TRACK_FUNCTION_CALLS
	if( initLevel == initializedLevel ) return true;
	if( !initLevel ) return false;	//initPlugin() must be called from outside first!
	initPlugin();	//retry once we have our options
	return initLevel == initializedLevel;
}


double GenericHistoryPlugin::recordPerformance( const shared_ptr<Activity>& activity ){
	TRACK_FUNCTION_CALLS
	Timestamp t_start = activity->time_start();
	Timestamp t_stop = activity->time_stop();
	OntologyAttributeID oaid = 0;	//According to datatypes/ids.hpp this is an illegal value. I hope that information is still up to date.

	// cout << "[GenericHistory]: " << t_start << " - " << t_stop << " @ ";

	if (t_stop == t_start) return 1/0.0;	//Infinite performance is sematically closer to an almost zero time operation than zero performance.

	// Find performance-relevant attribute's value for activity
	IGNORE_EXCEPTIONS(oaid = performanceAttIDs.at(activity->ucaid()););
	const vector<Attribute> & attributes = activity->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if (itr->id == oaid) {
			uint64_t value = itr->value.uint64();
			double result = ((double) value) / (t_stop - t_start);
			// cout << value << " => \t(Performance: " << result << ")" << endl;

			return result;
		}
	}

	// cout << "\t(Performance: NAN!)" << endl;
	return 0/0.0;	//return a NAN if the attribute was not found
}


vector<Attribute>* GenericHistoryPlugin::findCurrentHints( const shared_ptr<Activity>& activity, ActivityID* outParentId ) {
	TRACK_FUNCTION_CALLS
	const vector<ActivityID>& parents = activity->parentArray();
	vector<Attribute>* savedHints = 0;
	for( size_t i = parents.size(); i--; ) {
		IGNORE_EXCEPTIONS( savedHints = &openFileHints.at( parents[i] ); );
		if( savedHints ) {
			if(outParentId) *outParentId = parents[i];
			break;
		}
	}
	return savedHints;
}


void GenericHistoryPlugin::rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity ) {
	TRACK_FUNCTION_CALLS
	outHintVector->clear();
	const vector<Attribute>& attributes = activity->attributeArray();
	size_t hintCount = 0;
	for( size_t i = attributes.size(); i--; ) {
		OntologyAttributeFull curAttribute = facade->lookup_attribute_by_ID( attributes[i].id );
		IGNORE_EXCEPTIONS(
			hintTypes.at( attributes[i].id );
			outHintVector->emplace_back( attributes[i] );
			hintCount++;
		);
	}
	///@todo TODO: this is costly if Attributes contain strings since every assignment requires a free() and strdup()
	sort( outHintVector->begin(), outHintVector->end(), [](const Attribute& a, const Attribute& b){ return a.id < b.id; } );
}

void GenericHistoryPlugin::finalize() {
	multiplexer->unregisterCatchall( this, false );
	ActivityMultiplexerPlugin::finalize();
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new GenericHistoryPlugin();
	}
}
