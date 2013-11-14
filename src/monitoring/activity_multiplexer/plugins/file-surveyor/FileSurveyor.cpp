#include <iostream>
#include <sstream>
//#include <list>
#include <unordered_map>
#include <unordered_set>
//#include <algorithm>

#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>
//#include <knowledge/optimizer/OptimizerPluginInterface.hpp>
//#include <knowledge/optimizer/Optimizer.hpp>

#include <workarounds.hpp>

#include "FileSurveyorOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;


//const int kMinTrialCount = 5;

#define IGNORE_EXCEPTIONS(...) do { try { __VA_ARGS__ } catch(...) { } } while(0)
#define OUTPUT(...) cout << "[File Surveyor] " << __VA_ARGS__

enum TokenType {
	OPEN = 0,
	ACCESS,
	CLOSE,
	UNKNOWN,
	TOKEN_TYPE_COUNT
};
ADD_ENUM_OPERATORS(TokenType)


class FileSurvey {
	public:
		string fileName = "Kaiserin Sissi";
		Timestamp timeOpened = 0;
		Timestamp timeClosed = 0;
		//uint64_t fileHandle = 0;
		uint64_t filePosition = 0;
		uint64_t nAccesses = 0;
		uint64_t nAccessesSequential = 0;
		uint64_t nAccessesRandom = 0;
};


/*
 OBSOLETE COMMENT - UPDATE THIS!
 Some thoughts for an abstract plugin.

 This plugin remembers for every program, user, and filename (extensions) efficient and inefficient operations.
 It can be applied to any layer that supports the semantics of OPEN, ACCESS (i.e. read(), write(), etc.), optional HINTS and CLOSE.

 A configuration file defines the symbols which map to the OPEN etc. semantics and an integer descriptor as ontology identifier to track these.
 The time needed for the ACCESS semantics may depend on the abstract attributes: "size", offset (which defines some sort of regularity), and the set of supported HINTS.
 A hint may be set either on OPEN, during ACCESS.
 File extension, user and program are considered to behave similar as a hint.
 
 */
class FileSurveyorPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface {
	public:
		//FileSurveyorPlugin() : nTypes{} { }
		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		void Notify( shared_ptr<Activity> activity ) override;

		//virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) override;

		virtual ComponentReport prepareReport() override;

		//~FileSurveyorPlugin() { }

	private:
		int initLevel = 0;
		const int initializedLevel = -1;

		SystemInformationGlobalIDManager * sysinfo;

		string interface;
		string implementation;
		UniqueInterfaceID uiid;

		// OAIDs of various attributes
		OntologyAttributeID uidAttID;	// user-id
		OntologyAttributeID fnAttID;	// file name
		//OntologyAttributeID fhAttID;	// file handle
		OntologyAttributeID fpAttID;	// file position
		OntologyAttributeID btrAttID;	// bytes to read
		OntologyAttributeID btwAttID;	// bytes to write

		// Map ucaid to type of activity (Open/Access/Close)
		unordered_map<UniqueComponentActivityID, TokenType> types;

/*		// Map ucaid to ID of attribute used in computing activity performance
		unordered_map<UniqueComponentActivityID, OntologyAttributeID> performanceAttIDs;
*/
/*		// Map oaid of attribute to be used as hint to type of its value
		unordered_map<OntologyAttributeID,VariableDatatype::Type> hintTypes;
*/
//		TokenType lastActionType = UNKNOWN;

		unordered_map<ActivityID, FileSurvey > openFileSurveys;
		vector<FileSurvey> closedFileSurveys;

//		unordered_map<string, vector<HintPerformance> > userHints;
//		unordered_map<string, vector<HintPerformance> > appHints;
//		unordered_map<pair<string, string>, vector<HintPerformance> > userAppHints;
//		vector<HintPerformance> hints;

		// The set of file extensions we are to watch; we will ignore all others
		unordered_set<string> fileExtensionsToWatch;

		bool tryEnsureInitialization();	//If we are not initialized yet, try to do it. Returns true if initialization could be ensured.

		void openSurvey( shared_ptr<Activity> activity );
		void updateSurvey( shared_ptr<Activity> activity );
		void closeSurvey( shared_ptr<Activity> activity );

		const ActivityID * findParentAID( const shared_ptr<Activity>& activity );

/*		double recordPerformance( const shared_ptr<Activity>& activity );
		vector<Attribute>* findCurrentHints( const shared_ptr<Activity>& activity, ActivityID* outParentId );	//outParentId may be NULL
		void rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity );
*/
};



ComponentOptions * FileSurveyorPlugin::AvailableOptions() {
  return new FileSurveyorOptions();
}


void FileSurveyorPlugin::initPlugin() {
	#define RETURN_ON_EXCEPTION(...) do { try { __VA_ARGS__ } catch(...) { cerr << "[File Surveyor] initialization failed at initLevel = " << initLevel << "\n"; return; } } while(0)
	fprintf(stderr, "FileSurveyorPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);

	// Retrieve options
	FileSurveyorOptions & o = getOptions<FileSurveyorOptions>();

	switch(initLevel) {
		case 0:
			initLevel = 1;	//Once we are called from outside, there must exist sensible options for us, so if initialization fails now, we can simply retry.

		case 1:
			// Retrieve pointers to required modules
			//optimizer = GET_INSTANCE(Optimizer, o.optimizer);
			sysinfo = facade->get_system_information();
			assert(sysinfo);

			// Retrieve interface, implementation and uiid
			interface = o.interface;
			implementation = o.implementation;
			RETURN_ON_EXCEPTION( uiid = sysinfo->lookup_interfaceID(interface, implementation); );
			initLevel = 2;

		case 2:
			// Fill the token maps with every known activity's UCAID and type
			// First: OPEN calls
			RETURN_ON_EXCEPTION(
				for( auto itr = o.openTokens.begin(); itr != o.openTokens.end(); itr++ )
					types[sysinfo->lookup_activityID(uiid,*itr)] = OPEN;
			);
			initLevel = 3;

		case 3:
			// Second: ACCESS calls
			RETURN_ON_EXCEPTION(
				auto itr = o.accessTokens.begin();
				while (itr != o.accessTokens.end()) {
					UniqueComponentActivityID ucaid = sysinfo->lookup_activityID(uiid,*itr);
					types[ucaid] = ACCESS;
					itr++;
				}
			);
			initLevel = 4;

		case 4:
			// Third: CLOSE calls
			RETURN_ON_EXCEPTION(
				for( auto itr = o.closeTokens.begin(); itr != o.closeTokens.end(); itr++ )
					types[sysinfo->lookup_activityID(uiid,*itr)] = CLOSE;
			);
			initLevel = 5;

		case 5:
			// Gather the list of file extensions we are to watch into a map (for easy reference)
			RETURN_ON_EXCEPTION(
				for( auto itr = o.fileExtensionsToWatch.begin(); itr != o.fileExtensionsToWatch.end(); itr++ )
					fileExtensionsToWatch.emplace(*itr);
			);
/*			initLevel = 6;

		case 6:
			// Find oaids and value types for attributes used as hints and remember them
			RETURN_ON_EXCEPTION(
				for( auto itr = o.hintAttributes.begin(); itr != o.hintAttributes.end(); itr++ ) {
					string domain = itr->first;
					string attribute = itr->second;

//					cerr << "[File Surveyor] looking up attribute with domain \"" << domain << "\" and name \"" << attribute << "\", ";
					OntologyAttribute ontatt = facade->lookup_attribute_by_name(domain, attribute);
//					cerr << "received attribute ID " << ontatt.aID << "\n";
					hintTypes[ontatt.aID]=ontatt.storage_type;
					optimizer->registerPlugin( ontatt, this );	// Tell the optimizer to ask us for this attribute
				}
			);
*/			initLevel = 7;

		case 7:
			// Find and remember various other OAIDs
			RETURN_ON_EXCEPTION( uidAttID = facade->lookup_attribute_by_name("program","description/user-id").aID; );
			RETURN_ON_EXCEPTION( fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID; );
			//RETURN_ON_EXCEPTION( fhAttID = facade->lookup_attribute_by_name(interface,"descriptor/filehandle").aID; );
			RETURN_ON_EXCEPTION( fpAttID = facade->lookup_attribute_by_name(interface,"file/position").aID; );
			RETURN_ON_EXCEPTION( btrAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToRead").aID; );
			RETURN_ON_EXCEPTION( btwAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToWrite").aID; );
			initLevel = 8;
	}
	initLevel = initializedLevel;
}


void FileSurveyorPlugin::Notify( shared_ptr<Activity> activity ) {
	//OUTPUT( "received " << activity << endl )
	if( !tryEnsureInitialization() ) return;

	TokenType type = UNKNOWN;
	IGNORE_EXCEPTIONS( type = types.at( activity->ucaid() ); );

	cerr << "[File Surveyor] saw activity of type " << activity->ucaid() << ":\t";

	switch (type) {

	
		case OPEN:
			cerr << "open[" << activity->aid() << "]\n";
			openSurvey( activity );
			break;

		case ACCESS: {
			cerr << "access[" << activity->aid() << "]\n";
			updateSurvey( activity );
			break;
		}

		case CLOSE: {
			cerr << "close[" << activity->aid() << "]\n";
			closeSurvey( activity );
			break;
		}

		default:
			cerr << "(unknown)\n";
			break;
	}
}


/*
 * Create a new survey and initialize its data
 */
void FileSurveyorPlugin::openSurvey( shared_ptr<Activity> activity )
{
	OUTPUT( "openSurvey() received " << activity->aid() << endl );
	
	FileSurvey	survey;

	survey.timeOpened = activity->time_start();
	//survey.fileName = 

	openFileSurveys[ activity->aid() ] = survey;

	OUTPUT( "openSurveys size = " << openFileSurveys.size() << endl );
}


/*
 * Find proper survey for activity and update its data
 */
void FileSurveyorPlugin::updateSurvey( shared_ptr<Activity> activity )
{
	OUTPUT( "updateSurvey() received " << activity->aid() << endl );

	const ActivityID *	parentAID = findParentAID( activity );
	
	if( parentAID != NULL )
	{
		//FileSurvey * 	survey;

		//IGNORE_EXCEPTIONS( survey =  & openFileSurveys.at( *parentAID ); );
		//OUTPUT( "survey pointer = " << survey << endl );

		//OUTPUT( "nAccesses (before) = " << survey->nAccesses << endl );
		openFileSurveys.at( *parentAID ).nAccesses++;
		//survey->nAccesses++;
		//OUTPUT( "nAccesses  (after) = " << survey->nAccesses << endl );
		OUTPUT( "openSurveys size = " << openFileSurveys.size() << endl );
		OUTPUT( "closedSurveys size = " << closedFileSurveys.size() << endl );
	}
}




/*			vector<Attribute>* curHints = findCurrentHints( activity, 0 );
			if( curHints ) {
				double curPerformance = recordPerformance( activity );
				cout << "\t(Performance: " << curPerformance << ")" << endl;
				bool foundHints = false;
				for( size_t i = hints.size(); i--; ) {
					HintPerformance& temp = hints[i];
					if( temp.hints == *curHints ) {
						temp.averagePerformance = ( temp.averagePerformance*( temp.measurementCount ) + curPerformance )/( temp.measurementCount + 1 );
						temp.measurementCount++;
						foundHints = true;
						break;
					}
				}
				if( !foundHints ) {
					hints.emplace_back((HintPerformance){
						.hints = *curHints,
						.measurementCount = 1,
						.averagePerformance = curPerformance
					});
				}
			}
*/			


/*
 * Close survey object, finalize its statistics and move it to list of closed surveys
 */
void FileSurveyorPlugin::closeSurvey( shared_ptr<Activity> activity )
{
	OUTPUT( "closeSurvey() received " << activity->aid() << endl );

	const ActivityID *	parentAID = findParentAID( activity );
	OUTPUT( "Parent Array Size = " << activity->parentArray().size() << endl );
	//OUTPUT( "Parent AID * = " << parentAID << endl );

	if( parentAID != NULL )
	{
		FileSurvey 	survey = openFileSurveys.at( *parentAID );
	
		survey.timeClosed = activity->time_stop();

		openFileSurveys.erase( *parentAID );
		closedFileSurveys.push_back( survey );
		OUTPUT( "openSurveys size = " << openFileSurveys.size() << endl );
		OUTPUT( "closedSurveys size = " << closedFileSurveys.size() << endl );
	}
/*			ActivityID openFileHintsKey;
			if( findCurrentHints( activity, &openFileHintsKey ) ) {
				openFileHints.erase( openFileHintsKey );
			}
*/
}


/*OntologyValue FileSurveyorPlugin::optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) {
	if( initLevel != initializedLevel ) throw( NotFoundError() );

	const HintPerformance* result = 0;
	if( hints.size() && lastActionType != HINT ) {
		const HintPerformance* leastMeasurements = &hints[0];
		const HintPerformance* maxPerformance = &hints[0];
		for( size_t i = hints.size(); i --> 1; ) {
			if( hints[i].measurementCount < leastMeasurements->measurementCount ) leastMeasurements = &hints[i];
			if( hints[i].averagePerformance > maxPerformance->averagePerformance ) maxPerformance = &hints[i];
		}
		result = maxPerformance;
		if( leastMeasurements->measurementCount < kMinTrialCount ) result = leastMeasurements;
	}
	if( result && result->hints.size() ) {
		///@todo TODO: search for the right attribute
		return result->hints[0].value;
	}
	throw( NotFoundError() );
}
*/

ComponentReport FileSurveyorPlugin::prepareReport() {
	ComponentReport result;
	ostringstream reportText;

	if( !tryEnsureInitialization() ) return ComponentReport();

/*	for( auto itr = closedFileSurveys.begin(); itr != closedFileSurveys.end(); itr++ )
	{
		reportText << endl;
		reportText << "\"" << itr->fileName << "\"" << endl;
		reportText << "\n\tTime Opened:\t" << itr->timeOpened << endl;
		reportText << "\n\tTime Closed:\t" << itr->timeClosed << endl;
		reportText << "\n\tnAccesses:\t" << itr->nAccesses << endl;

		result.data[ itr->fileName ] = ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( reportText.str() ) );
	}
*/
	for( auto itr = openFileSurveys.begin(); itr != openFileSurveys.end(); itr++ )
	{
		reportText << endl;
		reportText << "\"" << itr->second.fileName << "\":" << endl;
		reportText << "\tTime Opened:\t" << itr->second.timeOpened << endl;
		reportText << "\tTime Closed:\t" << itr->second.timeClosed << endl;
		reportText << "\tnAccesses:\t" << itr->second.nAccesses << endl;

	}
	result.data[ "File Survey Report" ] = ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( reportText.str() ) );

	//result.data["File Surveyor"] = ReportEntry( ReportEntry::Type::SIOX_INTERNAL_CRITICAL, VariableDatatype( "Alles bella! ;)" ) );

	return result;
}


bool FileSurveyorPlugin::tryEnsureInitialization() {
	if( initLevel == initializedLevel ) return true;
	if( !initLevel ) return false;	//initPlugin() must be called from outside first!
	initPlugin();	//retry once we have our options
	return initLevel == initializedLevel;
}


/*double FileSurveyorPlugin::recordPerformance( const shared_ptr<Activity>& activity ){
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
*/


/*
 * Find the AID of the activity (if any) the activity is tied to via its parent relationship
 *
 * We can pass on the adress of the parent AID, as activity will keep it valid through its own lifetime
 */
const ActivityID * FileSurveyorPlugin::findParentAID( const shared_ptr<Activity>& activity )
{
	const vector<ActivityID>& parents = activity->parentArray();

	for( size_t i = parents.size(); i--; ) {
		if( openFileSurveys.count( parents[i] ) > 0 )
			return ( & parents[i] ); // Found it!
	}

	return NULL;
}


/*void FileSurveyorPlugin::rememberHints( vector<Attribute>* outHintVector, const shared_ptr<Activity>& activity ) {
	outHintVector->clear();
	const vector<Attribute>& attributes = activity->attributeArray();
	size_t hintCount = 0;
	for( size_t i = attributes.size(); i--; ) {
		OntologyAttribute curAttribute = facade->lookup_attribute_by_ID( attributes[i].id );
//		cerr << "Attribute " << attributes[i].id << " " << curAttribute.domain << "/" << curAttribute.name << " = " << attributes[i].value << "\n";
		IGNORE_EXCEPTIONS(
			hintTypes.at( attributes[i].id );
			outHintVector->emplace_back( attributes[i] );
			hintCount++;
		);
	}
//	cerr << "remembered " << hintCount << " hints from " << attributes.size() << " attributes\n";
	sort( outHintVector->begin(), outHintVector->end(), [](const Attribute& a, const Attribute& b){ return a.id < b.id; } );
}
*/

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileSurveyorPlugin();
	}
}
