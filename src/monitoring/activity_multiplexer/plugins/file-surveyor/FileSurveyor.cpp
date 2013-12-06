/**
 * An ActivityMultiplexerPlugin to watch the stream of incoming activites,
 * group information on the ones pertaining to the same files and report them
 * upon component shutdown.
 *
 * @author Michaela Zimmer, Nathanael Hübbe
 * @date 2013-12-06
 */

#include <iostream>
#include <sstream>
//#include <list>
#include <unordered_map>
#include <unordered_set>
//#include <algorithm>

#include <util/ExceptionHandling.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>

#include <workarounds.hpp>

#include "FileSurveyorOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;


#define OUTPUT(...) do { cout << "[File Surveyor] " << __VA_ARGS__ << "\n"; } while(0)



/*
 * Find difference between two uint64_t
 */
static inline const uint64_t absu64( uint64_t a, uint64_t b )
{
	if( a > b )
		return ( a - b );
	else
		return ( b - a );
}


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
		string fileName = "[NO VALID FILENAME]";
		string userID = "[NO VALID UID]";
		Timestamp timeOpened = 0;
		Timestamp timeClosed = 0;
		uint64_t filePosition = 0;
		uint64_t nAccessesRead = 0;
		uint64_t nAccessesReadSequential = 0;
		uint64_t nAccessesReadRandomShort = 0;
		uint64_t nAccessesReadRandomLong = 0;
		uint64_t nAccessesWrite = 0;
		uint64_t nAccessesWriteSequential = 0;
		uint64_t nAccessesWriteRandomShort = 0;
		uint64_t nAccessesWriteRandomLong = 0;
		uint64_t nBytesRead = 0;
		uint64_t nBytesWrite = 0;
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
		//FileSurveyorPlugin() { }

		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		void Notify( shared_ptr<Activity> activity ) override;

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
		OntologyAttributeID fpAttID;	// file position
		OntologyAttributeID btrAttID;	// bytes to read
		OntologyAttributeID btwAttID;	// bytes to write

		// Map ucaid to type of activity (Open/Access/Close)
		unordered_map<UniqueComponentActivityID, TokenType> types;

		unordered_map<ActivityID, FileSurvey > openFileSurveys;
		vector<FileSurvey> closedFileSurveys;

		// The set of file extensions we are to watch; we will ignore all others
		unordered_set<string> fileExtensionsToWatch;

		// The highest number of bytes that will still be classified as a "short jump";
		// anything above will be deemed a "long jump".
		uint64_t jumpSizeLimit;

		bool tryEnsureInitialization();	//If we are not initialized yet, try to do it. Returns true if initialization could be ensured.

		void openSurvey( shared_ptr<Activity> activity );
		void updateSurvey( shared_ptr<Activity> activity );
		void closeSurvey( shared_ptr<Activity> activity );

		const ActivityID * findParentAID( const shared_ptr<Activity>& activity );
		const Attribute * findAttributeByID( const shared_ptr<Activity>& activity, OntologyAttributeID oaid );
		const string findFileNameExtension( const string & fileName );
};



ComponentOptions * FileSurveyorPlugin::AvailableOptions() {
  return new FileSurveyorOptions();
}


void FileSurveyorPlugin::initPlugin() {
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
			jumpSizeLimit = o.jumpSizeLimit;
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
			RETURN_ON_EXCEPTION( );
*/			initLevel = 7;

		case 7:
			// Find and remember various other OAIDs
			RETURN_ON_EXCEPTION( uidAttID = facade->lookup_attribute_by_name("program","description/user-id").aID; );
			RETURN_ON_EXCEPTION( fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID; );
			RETURN_ON_EXCEPTION( fpAttID = facade->lookup_attribute_by_name(interface,"file/position").aID; );
			RETURN_ON_EXCEPTION( btrAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToRead").aID; );
			RETURN_ON_EXCEPTION( btwAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToWrite").aID; );
			initLevel = 8;
	}
	initLevel = initializedLevel;
}


void FileSurveyorPlugin::Notify( shared_ptr<Activity> activity ) {
	//OUTPUT( "received " << activity );
	if( !tryEnsureInitialization() ) return;

	TokenType type = UNKNOWN;
	IGNORE_EXCEPTIONS( type = types.at( activity->ucaid() ); );

	//OUTPUT( "[File Surveyor] saw activity of type " << activity->ucaid() << ":\t" );

	switch (type) {

		case OPEN:
			//OUTPUT( "open[" << activity->aid() << "]\n" );
			openSurvey( activity );
			break;

		case ACCESS: {
			//OUTPUT( "access[" << activity->aid() << "]\n" );
			updateSurvey( activity );
			break;
		}

		case CLOSE: {
			//OUTPUT( "close[" << activity->aid() << "]\n" );
			closeSurvey( activity );
			break;
		}

		default:
			//OUTPUT( "(unknown)\n" );
			break;
	}
}


/*
 * Create a new survey and initialize its data
 */
void FileSurveyorPlugin::openSurvey( shared_ptr<Activity> activity )
{
	//OUTPUT( "openSurvey() received activity " << activity->aid() );

	// Find file name and check whether extension matches pattern given in configuration
	const Attribute * attFileName = findAttributeByID( activity, fnAttID );
	if( attFileName == NULL )
	{
		// This should not happen; it is, however, no serious problem.
		cerr << "[FileSurvey] No valid file name in activity " << activity->aid() << "!" << endl;
		return;
	}
	const string fileExtension = findFileNameExtension(attFileName->value.str());
	//OUTPUT( "File extension found: " << fileExtension );
	if( fileExtensionsToWatch.count( fileExtension ) > 0 )
	{
		// Create and fill new survey for the file just opened

		FileSurvey	survey;

		// Start time
		survey.timeOpened = activity->time_start();
		// File name
		survey.fileName = attFileName->value.str();
		// User ID
		// @todo TODO: Retrieve correct UID for OPEN activities.
		const Attribute * attUserID = findAttributeByID( activity, uidAttID );
		if ( attUserID != NULL )
		{
			survey.userID = attUserID->value.str();
			OUTPUT( "UserID: " << attUserID->value.str() );
		}
		else
			OUTPUT( "No valid user ID in activity" << activity->aid() << "!" << endl );

		openFileSurveys[ activity->aid() ] = survey;

	}
	//OUTPUT( "openSurveys size = " << openFileSurveys.size() );
}


/*
 * Find proper survey for activity and update its data
 */
void FileSurveyorPlugin::updateSurvey( shared_ptr<Activity> activity )
{
	//OUTPUT( "updateSurvey() received activity " << activity->aid() );

	const ActivityID *	parentAID = findParentAID( activity );

	if( parentAID != NULL )
	{
		FileSurvey * survey;

		try {
			survey = & openFileSurveys.at( *parentAID );
		}
		catch( NotFoundError ) {
			// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost to // SIOX, or when the file name did not match the pattern given in the configuration.
			// Therefore, disregard all following activities on those files until closed and re-opened.
			// cerr << "[FileSurvey]: No parent activity found for activity " << activity->aid() << "!" << endl;
			return;
		}

		// Find file position to distinguish between random (short or long jump) and sequential accesses
		const Attribute * attFilePointer = findAttributeByID( activity, fpAttID );

		// Process BytesToRead or BytesToWrite, assuming exactly one of both is set
		// First try: BytesToRead
		const Attribute * attBytesProcessed = findAttributeByID( activity, btrAttID );
		if( attBytesProcessed != NULL )
		{
			uint64_t nBytesProcessed = attBytesProcessed->value.uint64();

			// Update byte count
			survey->nBytesRead += nBytesProcessed;
			// Move memorized file pointer accordingly
			survey->filePosition += attBytesProcessed->value.uint64();
			// Update access counts according to file pointer position
			if( attFilePointer != NULL )
			{
				uint64_t fp = attFilePointer->value.uint64();
				uint64_t fpJump = absu64(fp, survey->filePosition);

				if( fpJump > 0 )
				{
					survey->filePosition = fp;
					if ( fpJump > jumpSizeLimit )
						survey->nAccessesReadRandomLong++;
					else
						survey->nAccessesReadRandomShort++;
				}
				else
					survey->nAccessesReadSequential++;
				//OUTPUT( "file pointer = " << fp );
			}
			else
			{
				survey->nAccessesReadSequential++;
			}

			return;
		}

		// Second try: BytesToWrite
		attBytesProcessed = findAttributeByID( activity, btwAttID );
		if (attBytesProcessed != NULL )
		{
			uint64_t nBytesProcessed = attBytesProcessed->value.uint64();

			// Update byte count
			survey->nBytesWrite += nBytesProcessed;
			// Move memorized file pointer accordingly
			survey->filePosition += nBytesProcessed;
			// Update access counts according to file pointer position
			if( attFilePointer != NULL )
			{
				uint64_t fp = attFilePointer->value.uint64();
				uint64_t fpJump = absu64(fp, survey->filePosition);

				if( fpJump > 0 )
				{
					survey->filePosition = fp;
					if ( fpJump > jumpSizeLimit )
						survey->nAccessesWriteRandomLong++;
					else
						survey->nAccessesWriteRandomShort++;
				}
				else
					survey->nAccessesWriteSequential++;
				//OUTPUT( "file pointer = " << fp );
			}
			else
			{
				survey->nAccessesWriteSequential++;
			}

			return;
		}

		// Report that neither was found
		cerr << "[FileSurvey] No size of payload data attribute found in activity " << activity->aid() << "!" << endl;

		//OUTPUT( "openSurveys size = " << openFileSurveys.size() );
		//OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
	}
}


/*
 * Close survey object, finalize its statistics and move it to list of closed surveys
 */
void FileSurveyorPlugin::closeSurvey( shared_ptr<Activity> activity )
{
	//OUTPUT( "closeSurvey() received activity " << activity->aid() );

	const ActivityID *	parentAID = findParentAID( activity );
	//OUTPUT( "Parent Array Size = " << activity->parentArray().size() );
	//OUTPUT( "Parent AID * = " << parentAID );

	if( parentAID != NULL )
	{
		FileSurvey survey;

		try {
			survey = openFileSurveys.at( *parentAID );
		}
		catch( NotFoundError ) {
			// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost to // SIOX, or when the file name did not match the pattern given in the configuration.
			// Therefore, disregard all following activities on those files until closed and re-opened.
			//cerr << "[FileSurvey]: No parent activity found for activity " << activity->aid() << "!" << endl;
			return;
		}

		survey.timeClosed = activity->time_stop();

		openFileSurveys.erase( *parentAID );
		closedFileSurveys.push_back( survey );
		//OUTPUT( "openSurveys size = " << openFileSurveys.size() );
		//OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
	}
}


ComponentReport FileSurveyorPlugin::prepareReport()
{
	ComponentReport result;

	GroupEntry * gePlugin = new GroupEntry("FileSurveyorPlugin");
	ostringstream reportText;

	if( !tryEnsureInitialization() ) return ComponentReport();

	for( auto itr = closedFileSurveys.begin(); itr != closedFileSurveys.end(); itr++ )
	{
		uint64_t nAccessesReadRandom = itr->nAccessesReadRandomShort + itr->nAccessesReadRandomLong;
		uint64_t nAccessesRead = nAccessesReadRandom + itr->nAccessesReadSequential;
		uint64_t nAccessesWriteRandom = itr->nAccessesWriteRandomShort + itr->nAccessesWriteRandomLong;
		uint64_t nAccessesWrite = nAccessesWriteRandom + itr->nAccessesWriteSequential;
		uint64_t nAccesses = nAccessesRead + nAccessesWrite;
		uint64_t nBytes = itr->nBytesRead + itr->nBytesWrite;

		double nBytesReadAverage = 0.0;
		if( nAccessesRead > 0 )
			nBytesReadAverage = itr->nBytesRead / ((double) nAccessesRead);
		double nBytesWriteAverage = 0.0;
		if( nAccessesWrite > 0 )
			nBytesWriteAverage = ((double) itr->nBytesWrite) / ((double) nAccessesWrite);


		GroupEntry * geFile = new GroupEntry( itr->fileName, gePlugin );

		// General
		result.addEntry( new GroupEntry( "User ID", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->userID ) ));
		result.addEntry( new GroupEntry( "Time opened", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->timeOpened ) ));
		result.addEntry( new GroupEntry( "Time closed", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->timeClosed ) ));

		// # Accesses
		GroupEntry * geAccesses = new GroupEntry( "Accesses", geFile );
		result.addEntry( geAccesses, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nAccesses ) ));
		// Reading
		GroupEntry * geReading = new GroupEntry( "Reading", geAccesses );
		result.addEntry( geReading, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesRead ) ));
		result.addEntry( new GroupEntry( "Sequential", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesReadSequential ) ));
		result.addEntry( new GroupEntry( "Random, short seek", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesReadRandomShort ) ));
		result.addEntry( new GroupEntry( "Random, long seek", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesReadRandomLong ) ));
		// Writing
		GroupEntry * geWriting = new GroupEntry( "Writing", geAccesses );
		result.addEntry( geWriting, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesWrite ) ));
		result.addEntry( new GroupEntry( "Sequential", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesWriteSequential ) ));
		result.addEntry( new GroupEntry( "Random, short seek", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesWriteRandomShort ) ));
		result.addEntry( new GroupEntry( "Random, long seek", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nAccessesWriteRandomLong ) ));

		// # Bytes
		GroupEntry * geBytes = new GroupEntry( "Bytes", geFile );
		result.addEntry( geBytes, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytes ) ));
		result.addEntry( new GroupEntry( "Total read", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nBytesRead ) ));
		result.addEntry( new GroupEntry( "Read per access", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytesReadAverage ) ));
		result.addEntry( new GroupEntry( "Total written", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( itr->nBytesWrite ) ));
		result.addEntry( new GroupEntry( "Written per access", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytesWriteAverage ) ));

		//reportText << endl;
		//reportText << "\t\"" << itr->fileName << "\":" << endl;
		//reportText << "\t\tUser ID:    \t" << itr->userID << endl;
		//reportText << "\t\tTime opened:\t" << itr->timeOpened << endl;
		//reportText << "\t\tTime closed:\t" << itr->timeClosed << endl;
		//reportText << "\t\tnAccesses:\t" << nAccesses << endl;
		//reportText << "\t\t\tReading:\t" << nAccessesRead << endl;
		//reportText << "\t\t\t\tSequential:\t" << itr->nAccessesReadSequential << endl;
		//reportText << "\t\t\t\tRandom:    \t" << nAccessesReadRandom << endl;
		//reportText << "\t\t\t\t\tshort seek:\t" << itr->nAccessesReadRandomShort << endl;
		//reportText << "\t\t\t\t\tlong seek: \t" << itr->nAccessesReadRandomLong << endl;
		//reportText << "\t\t\tWriting:\t" << nAccessesWrite << endl;
		//reportText << "\t\t\t\tSequential:\t" << itr->nAccessesWriteSequential << endl;
		//reportText << "\t\t\t\tRandom:    \t" << nAccessesWriteRandom << endl;
		//reportText << "\t\t\t\t\tshort seek:\t" << itr->nAccessesWriteRandomShort << endl;
		//reportText << "\t\t\t\t\tlong seek: \t" << itr->nAccessesWriteRandomLong << endl;
		//reportText << "\t\tnBytes:\t" << nBytes << endl;
		//reportText << "\t\t\tTotal read:        \t" << itr->nBytesRead << endl;
		//reportText << "\t\t\tRead per access:   \t" << nBytesReadAverage << endl;
		//reportText << "\t\t\tTotal written:     \t" << itr->nBytesWrite << endl;
		//reportText << "\t\t\tWritten per access:\t" << nBytesWriteAverage << endl;
	}
	//result.addEntry( gePlugin, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( reportText.str() ) ));

	return result;
}


bool FileSurveyorPlugin::tryEnsureInitialization()
{
	if( initLevel == initializedLevel ) return true;
	if( !initLevel ) return false;	//initPlugin() must be called from outside first!
	initPlugin();	//retry once we have our options
	return initLevel == initializedLevel;
}


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


/**
 * Find the value of the given attribute for the given activity.
 *
 * Search the activity's attributes for one matching the given AID.
 * If found, return its value; otherwise, return @c NULL.
 *
 * @param activity The activity to be investigated
 * @param oaid The OAID of the attribute to look for
 * @return A constant pointer to the attribute in the activity's data, or @c NULL if not found.
 */
 const Attribute * FileSurveyorPlugin::findAttributeByID( const shared_ptr<Activity>& activity, OntologyAttributeID oaid )
 {
 	const vector<Attribute> & attributes = activity->attributeArray();

	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if( itr->id == oaid )
			return &( *itr );
	}
	return NULL;
 }


/*
 * Find the extension of the file name given.
 */
const string FileSurveyorPlugin::findFileNameExtension( const string & fileName )
{
	string::size_type	position;

	position = fileName.rfind( '.' );

	if( position != string::npos )
	    return fileName.substr( position + 1 );
	else
		return "";
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileSurveyorPlugin();
	}
}
