/**
 * An ActivityMultiplexerPlugin to watch the stream of incoming activites,
 * group information on the ones pertaining to the same (user, file) combination
 * and report them upon component shutdown.
 * It can be applied to any layer that supports the semantics of OPEN, READ, WRITE, SEEK and CLOSE.
 * A configuration file defines the symbols which map to the OPEN etc. semantics
 * and an integer limit to differentiate between short and long SEEKs.
 * The byte counts refer to byte actually processed; only if no reliable attribute is available,
 * will the system fall back to the attributes describing the number of bytes that were to be
 * processed as per the activity's original call.
 *
 * @author Michaela Zimmer, Nathanael Hübbe, Julian Kunkel
 * @date 2014-03-06
 */

#include <iostream>
#include <sstream>
#include <list>
// #include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <mutex>

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


#define OUTPUT(...) do { cout << "[File Surveyor @ " << interface << "] " << __VA_ARGS__ << "\n"; } while(0)
#define ERROR(...) do { cerr << "[File Surveyor @ " << interface << "] " << __VA_ARGS__ << "!\n"; } while(0)



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
	SEEK,
	READ,
	WRITE,
	CLOSE,
	UNKNOWN,
	TOKEN_TYPE_COUNT
};
ADD_ENUM_OPERATORS(TokenType)


class FileSurvey {
	public:
		// Used to differentiate surveys
		string fileName = "[NO VALID FILENAME]";
		//string userID = "[NO VALID UID]";
		// Counters for current open-access-close cycle
		Timestamp timeFirstOpened = 0;
		Timestamp timeOpened = 0;
		Timestamp timeClosed = 0;
		uint64_t filePositionAfterLastAccess = 0; // Both have to be tracked, as seeks are executed only
		uint64_t filePositionAfterLastSeek = 0; // virtually, until the next access forces the net sum seek
		// Aggregated statistics over all cycles
		Timestamp timeTotalOpen = 0;
		Timestamp timeTotalSeek = 0;
		Timestamp timeTotalRead = 0;
		Timestamp timeTotalWrite = 0;
		Timestamp timeTotalClose = 0;
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
		uint64_t nOpens = 0;
		uint64_t nCloses = 0;
		uint64_t nSeeks = 0; // True seeks only; no accesses with fp attribute
		uint64_t nSeeksShort = 0;
		uint64_t nSeeksLong = 0;
		uint64_t nTotalSeekDistance = 0;
		uint64_t nTotalSeekDistanceLast = 0; // Total seek distance since last access
		uint64_t nTotalSeekDistanceRead = 0;
		uint64_t nTotalSeekDistanceWrite = 0;
};


class FileSurveyorPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface {
	public:
		// FileSurveyorPlugin() { }

		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		void Notify( const shared_ptr<Activity> & activity, int lost );

		virtual ComponentReport prepareReport() override;
		void finalize() override;
		//~FileSurveyorPlugin() { }

	private:
		int initLevel = 0;
		const int initializedLevel = -1;

		SystemInformationGlobalIDManager * sysinfo;

		// Information about the layer we are to watch
		string interface = "(yet unknown)";
		string implementation = "(yet unknown)";
		UniqueInterfaceID uiid;

		// OAIDs of various attributes
		OntologyAttributeID uidAttID;	// user-id
		OntologyAttributeID fnAttID;	// file name
		OntologyAttributeID fpAttID;	// file position
		OntologyAttributeID btrAttID;	// bytes to read
		OntologyAttributeID btwAttID;	// bytes to write
		OntologyAttributeID brAttID;	// bytes actually read 		// FIXME: Einbauen!!!
		OntologyAttributeID bwAttID;	// bytes actually written 	// FIXME: Einbauen!!!

		// Map ucaid to type of activity (Open/Read/Write/Close)
		unordered_map<UniqueComponentActivityID, TokenType> types;

		// The set of file extensions we are to watch; we will ignore all others
		unordered_set<string> fileExtensionsToWatch;

		// The highest number of bytes that will still be classified as a "short seek";
		// anything above will be deemed a "long seek".
		uint64_t seekSizeLimit;

		// What to report; setting both to false will switch off reporting
		bool individualReports = false;
		bool aggregatedReport = false;


		// Collections for closed and still opened surveys
		mutex  collectionMutex; // To protect access to collections
		list<FileSurvey> closedFileSurveys;
		unordered_map<ActivityID, FileSurvey> openFileSurveys; // Key: AID of last open() to file
		// unordered_map<pair<string,string>, FileSurvey> closedFileSurveys; // TODO Key: (UserID, Filename)


		bool tryEnsureInitialization();	//If we are not initialized yet, try to do it. Returns true if initialization could be ensured.

		void openSurvey( const shared_ptr<Activity> & activity, int lost  );
		void updateSurvey( const shared_ptr<Activity> & activity, int lost  );
		void updateSurveySeek( const shared_ptr<Activity> & activity, int lost  );
		void updateSurveyRead( const shared_ptr<Activity> & activity, int lost  );
		void updateSurveyWrite( const shared_ptr<Activity> & activity, int lost  );
		void closeSurvey( const shared_ptr<Activity> & activity, int lost  );

		const FileSurvey aggregateSurveys();
		void reportSurvey(ComponentReport & report, const FileSurvey & survey);

		const UniqueComponentActivityID lookupUCAID( const string & activity );
		const ActivityID * findParentAID( const shared_ptr<Activity>& activity );
		const Attribute * findAttributeByID( const shared_ptr<Activity>& activity, OntologyAttributeID oaid );
		const string findFileNameExtension( const string & fileName );
};


ComponentOptions * FileSurveyorPlugin::AvailableOptions() {
  return new FileSurveyorOptions();
}


/**
 * Turn a string in-place into uppercase
 */
static void toUpper(string & ext){
	for_each( ext.begin(), ext.end(), [](char&c){
		c = std::toupper(c);
	});
}


void FileSurveyorPlugin::initPlugin() {
	// fprintf(stderr, "FileSurveyorPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);

	// Retrieve options
	FileSurveyorOptions & o = getOptions<FileSurveyorOptions>();

	switch(initLevel) {
		case 0:
			// OUTPUT( "Checkpoint " << initLevel );
			initLevel = 1;	//Once we are called from outside, there must exist sensible options for us, so if initialization fails now, we can simply retry.

		case 1:
			// OUTPUT( "Checkpoint " << initLevel );
			// Retrieve pointers to required modules
			//optimizer = GET_INSTANCE(Optimizer, o.optimizer);
			sysinfo = facade->get_system_information();
			assert(sysinfo);

			// Retrieve interface, implementation and uiid
			interface = o.interface;
			implementation = o.implementation;
			seekSizeLimit = o.seekSizeLimit;
			RETURN_ON_EXCEPTION( uiid = sysinfo->lookup_interfaceID(interface, implementation); );

			// Retrieve report detail levels
			individualReports = o.individualReports;
			aggregatedReport = o.aggregatedReport;

			initLevel = 2;

		case 2:
			// OUTPUT( "Checkpoint " << initLevel );
			// Fill the token maps with every known activity's UCAID and type
			// First: OPEN calls
			for( auto token : o.openTokens )
			{
				UniqueInterfaceID ucaid = lookupUCAID( token );
				if ( ucaid != 0 )
					multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &FileSurveyorPlugin::openSurvey ), false );
				else
					return;
			}
			initLevel = 3;

		case 3:
			// OUTPUT( "Checkpoint " << initLevel );
			// Second: SEEK calls
			for( auto token : o.seekTokens )
			{
				UniqueInterfaceID ucaid = lookupUCAID( token );
				if ( ucaid != 0 )
					multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &FileSurveyorPlugin::updateSurveySeek ), false );
				else
					return;
			}
			initLevel = 4;

		case 4:
			// OUTPUT( "Checkpoint " << initLevel );
			// Third: READ calls
			for( auto token : o.readTokens )
			{
				UniqueInterfaceID ucaid = lookupUCAID( token );
				if ( ucaid != 0 )
					multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &FileSurveyorPlugin::updateSurveyRead ), false );
				else
					return;
			}
			initLevel = 5;

		case 5:
			// OUTPUT( "Checkpoint " << initLevel );
			// Fourth: WRITE calls
			for( auto token : o.writeTokens )
			{
				UniqueInterfaceID ucaid = lookupUCAID( token );
				if ( ucaid != 0 )
					multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &FileSurveyorPlugin::updateSurveyWrite ), false );
				else
					return;
			}
			initLevel = 6;

		case 6:
			// OUTPUT( "Checkpoint " << initLevel );
			// Fifth: CLOSE calls
			for( auto token : o.closeTokens )
			{
				UniqueInterfaceID ucaid = lookupUCAID( token );
				if ( ucaid != 0 )
					multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &FileSurveyorPlugin::closeSurvey ), false );
				else
					return;
			}
			initLevel = 7;

		case 7:
			// OUTPUT( "Checkpoint " << initLevel );
			// Gather the list of file extensions we are to watch into a map (for easy reference)
			for( auto extension : o.fileExtensionsToWatch ){
				string ext = extension;
				toUpper(ext);
				fileExtensionsToWatch.emplace(ext);
			}
			initLevel = 8;

		case 8:
			// OUTPUT( "Checkpoint " << initLevel );
			// Find and remember various other OAIDs
			RETURN_ON_EXCEPTION( uidAttID = facade->lookup_attribute_by_name("program","description/user-id").aID; );
			RETURN_ON_EXCEPTION( fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID; );
			RETURN_ON_EXCEPTION( fpAttID = facade->lookup_attribute_by_name(interface,"file/position").aID; );
			RETURN_ON_EXCEPTION( btrAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToRead").aID; );
			RETURN_ON_EXCEPTION( btwAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesToWrite").aID; );
			RETURN_ON_EXCEPTION( brAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesRead").aID; );
			RETURN_ON_EXCEPTION( bwAttID = facade->lookup_attribute_by_name(interface,"quantity/BytesWritten").aID; );
			initLevel = 9;
	}
	initLevel = initializedLevel;
}


/*
 * Receive notification of newly witnessed activity.
 * lost, the number of activities missed since the last one, is ignored.
 */
void FileSurveyorPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	//OUTPUT( "received " << activity );
	if( !tryEnsureInitialization() ) return;
/*
	TokenType type = UNKNOWN;
	IGNORE_EXCEPTIONS( type = types.at( activity->ucaid() ); );
*/
	OUTPUT( "saw activity of type " << activity->ucaid() << ":" );
/*
	switch (type) {

		case OPEN:
			OUTPUT( "open[" << activity->aid() << "]" );
			openSurvey( activity, lost );
			break;

		case SEEK:
			OUTPUT( "seek[" << activity->aid() << "]" );
			updateSurveySeek( activity, lost );
			break;

		case READ:
			OUTPUT( "read[" << activity->aid() << "]" );
			updateSurveyRead( activity, lost );
			break;

		case WRITE:
			OUTPUT( "write[" << activity->aid() << "]" );
			updateSurveyWrite( activity, lost );
			break;

		case CLOSE:
			OUTPUT( "close[" << activity->aid() << "]" );
			closeSurvey( activity, lost );
			break;
*/
		// default:
			string aName = sysinfo->lookup_activity_name( activity->ucaid() );
			OUTPUT( "(unknown: " << aName << ")[" << activity->aid() << "]" );
			// break;
	// }
}


/*
 * Create a new survey and initialize its data
 */
void FileSurveyorPlugin::openSurvey( const shared_ptr<Activity> & activity, int lost  )
{
	if( !tryEnsureInitialization() ) return;

	// OUTPUT( "openSurvey() received activity " << activity->aid() << ": " << sysinfo->lookup_activity_name( activity->ucaid()) );

	// Find file name and check whether extension matches pattern given in configuration
	const Attribute * attFileName = findAttributeByID( activity, fnAttID );
	if( attFileName == NULL )
	{
		// This should not happen; it is, however, no serious problem; just don't do anything.
		ERROR( "No valid file name in activity " << activity->aid() );
		return;
	}
	string fileName = attFileName->value.str();
	// OUTPUT( "File name found: " << fileName );
	string fileExtension = findFileNameExtension(fileName);
	toUpper(fileExtension);
	// OUTPUT( "File extension found: " << fileExtension );
	if( fileExtensionsToWatch.count( fileExtension ) > 0 || fileExtensionsToWatch.empty() )
	{
		// Retrieve User ID
		//string userID = "(no valid UserID)";
		// TODO: Retrieve correct UID for OPEN activities.
		// TODO: Add application name/path (and PID?) to tuple by which to differentiate surveys
		//const Attribute * attUserID = findAttributeByID( activity, uidAttID );
		//if ( attUserID == NULL ){
		//	ERROR( "No valid user ID in activity" << activity->aid() );
		//}
		//else{
		//	userID = attUserID->value.str();
			// OUTPUT( "UserID: " << userID );
		//}

		FileSurvey	survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			// See whether there is a survey for this user and file already that can be reopened
			for(auto candidate=closedFileSurveys.begin(); candidate != closedFileSurveys.end(); candidate++)
			{
				if( //candidate->userID == attUserID->value.str() &&
				   candidate->fileName == attFileName->value.str())
				{
					// Reuse closed survey

					// Update data
					survey = *candidate;
					survey.nOpens++;
					survey.timeOpened = activity->time_start();
					survey.filePositionAfterLastAccess = 0;
					survey.filePositionAfterLastSeek = 0;
					survey.timeTotalOpen += activity->time_stop() - activity->time_start();

					// Move survey back to open collection
					openFileSurveys[ activity->aid() ] = survey;
					closedFileSurveys.erase(candidate);

					return;
				}
			}

			// Create and fill new survey for the file just opened
			survey.nOpens = 1;
			survey.timeFirstOpened = survey.timeOpened = activity->time_start();
			survey.fileName = attFileName->value.str();
			//survey.userID = userID;
			survey.timeTotalOpen += activity->time_stop() - activity->time_start();

			openFileSurveys[ activity->aid() ] = survey;
		} // collectionLock
	}
	// OUTPUT( "openSurveys size = " << openFileSurveys.size() );
	// OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
}


/*
 * Update survey after receiving a seek() class activity
 */
void FileSurveyorPlugin::updateSurveySeek( const shared_ptr<Activity> & activity, int lost  )
{
	if( !tryEnsureInitialization() ) return;

	// OUTPUT( "updateSurveySeek() received activity " << activity->aid() << ": " << sysinfo->lookup_activity_name( activity->ucaid()) );

	const ActivityID *	parentAID = findParentAID( activity );

	if( parentAID != NULL )
	{
		FileSurvey * survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			try {
				survey = & openFileSurveys.at( *parentAID );
			}
			catch( NotFoundError ) {
				// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost
				// to SIOX, or when the file name did not match the pattern given in the configuration.
				// Therefore, disregard all following activities on those files until closed and re-opened.
				// ERROR( "No parent activity found for activity " << activity->aid() << "!" );
				return;
			}

			// Find file position to distinguish between random (short or long jump) and sequential accesses
			const Attribute * attFilePointer = findAttributeByID( activity, fpAttID );

			if( attFilePointer != NULL )
			{
				// Update access counts according to file position
				uint64_t fp = attFilePointer->value.uint64();
				uint64_t fpJump = absu64(fp, survey->filePositionAfterLastSeek);
				// OUTPUT( "True seek to " << fp << "(Distance: " << fpJump << ")" );

				if ( fpJump > seekSizeLimit )
					survey->nSeeksLong++;
				else
					survey->nSeeksShort++;
				survey->nTotalSeekDistance += fpJump;
				survey->nTotalSeekDistanceLast += fpJump;

				// Update memorized file position
				survey->filePositionAfterLastSeek = fp;

				// OUTPUT( "file position = " << fp );
			}
			else
			{
				ERROR( "No destination file pointer attribute found in activity " << activity->aid() );
			}

			// Update statistics
			survey->nSeeks++;
			survey->timeTotalSeek += activity->time_stop() - activity->time_start();
		} // collectionLock

		// OUTPUT( "openSurveys size = " << openFileSurveys.size() );
		// OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
	}
}


/*
 * Update survey after receiving a read() class activity
 */
void FileSurveyorPlugin::updateSurveyRead( const shared_ptr<Activity> & activity, int lost  )
{
	if( !tryEnsureInitialization() ) return;

	// OUTPUT( "updateSurveyRead() received activity " << activity->aid() << ": " << sysinfo->lookup_activity_name( activity->ucaid()) );

	const ActivityID *	parentAID = findParentAID( activity );

	if( parentAID != NULL )
	{
		FileSurvey * survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			try {
				survey = & openFileSurveys.at( *parentAID );
			}
			catch( NotFoundError ) {
				// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost
				// to SIOX, or when the file name did not match the pattern given in the configuration.
				// Therefore, disregard all following activities on those files until closed and re-opened.
				// ERROR( "No parent activity found for activity " << activity->aid() << "!" );
				return;
			}

			// Process BytesToRead
			const Attribute * attBytesProcessed = findAttributeByID( activity, btrAttID );
			if( attBytesProcessed != NULL )
			{
				uint64_t nBytesProcessed = attBytesProcessed->value.uint64();
				survey->nBytesRead += nBytesProcessed;

				// Find destination file position (if given)
				const Attribute * attFilePointer = findAttributeByID( activity, fpAttID );
				uint64_t fp = 0;
				if( attFilePointer != NULL )
				{
					// Use fp given here
					fp = attFilePointer->value.uint64();
					// Update general seek data
					// TODO: Add to the running count of (true) seeks or not?
				}
				else
				{
					// Use fp resulting from previous seeks
					fp = survey->filePositionAfterLastSeek;
				}
				//OUTPUT( "file position = " << fp );

				// Update access counts according to file position
				uint64_t fpJump = absu64(fp, survey->filePositionAfterLastAccess);
				// if( attFilePointer != NULL )
					// OUTPUT( "Read-implicit seek to " << fp << "(Distance: " << fpJump << ")" );

				if( fpJump > 0 )
				{
					survey->filePositionAfterLastAccess = fp;
					if ( fpJump > seekSizeLimit )
						survey->nAccessesReadRandomLong++;
					else
						survey->nAccessesReadRandomShort++;
					survey->nTotalSeekDistanceRead += fpJump;
				}
				else
					survey->nAccessesReadSequential++;

				// Update memorized file position
				survey->filePositionAfterLastAccess = fp + nBytesProcessed;
				survey->filePositionAfterLastSeek = survey->filePositionAfterLastAccess;
			}
			else
			{
				// Empty payload data?
				ERROR( "No size of payload data attribute found in activity " << activity->aid() );
			}

			// Update statistics
			survey->nAccessesRead++;
			survey->timeTotalRead += activity->time_stop() - activity->time_start();
		} // collectionLock

		// OUTPUT( "openSurveys size = " << openFileSurveys.size() );
		// OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
	}
}


/*
 * Update survey after receiving a write() class activity
 */
void FileSurveyorPlugin::updateSurveyWrite( const shared_ptr<Activity> & activity, int lost  )
{
	if( !tryEnsureInitialization() ) return;

	// OUTPUT( "updateSurveyWrite() received activity " << activity->aid() << ": " << sysinfo->lookup_activity_name( activity->ucaid()) );

	const ActivityID *	parentAID = findParentAID( activity );

	if( parentAID != NULL )
	{
		FileSurvey * survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			try {
				survey = & openFileSurveys.at( *parentAID );
			}
			catch( NotFoundError ) {
				// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost
				// to SIOX, or when the file name did not match the pattern given in the configuration.
				// Therefore, disregard all following activities on those files until closed and re-opened.
				// ERROR( "No parent activity found for activity " << activity->aid() );
				return;
			}

			// Process BytesToWrite
			const Attribute * attBytesProcessed = findAttributeByID( activity, btwAttID );
			if( attBytesProcessed != NULL )
			{
				uint64_t nBytesProcessed = attBytesProcessed->value.uint64();
				survey->nBytesWrite += nBytesProcessed;

				// Find destination file position (if given)
				const Attribute * attFilePointer = findAttributeByID( activity, fpAttID );
				uint64_t fp = 0;
				if( attFilePointer != NULL )
				{
					// Use fp given here
					fp = attFilePointer->value.uint64();
					// Update general seek data
					// TODO: Add to the running count of (true) seeks or not?
				}
				else
				{
					// Use fp resulting from previous seeks
					fp = survey->filePositionAfterLastSeek;
				}
				//OUTPUT( "file position = " << fp );

				// Update access counts according to file position
				uint64_t fpJump = absu64(fp, survey->filePositionAfterLastAccess);
				// if( attFilePointer != NULL )
					// OUTPUT( "Write-implicit seek to " << fp << "(Distance: " << fpJump << ")" );

				if( fpJump > 0 )
				{
					survey->filePositionAfterLastAccess = fp;
					if ( fpJump > seekSizeLimit )
						survey->nAccessesWriteRandomLong++;
					else
						survey->nAccessesWriteRandomShort++;
					survey->nTotalSeekDistanceWrite += fpJump;
				}
				else
					survey->nAccessesWriteSequential++;

				// Update memorized file position
				survey->filePositionAfterLastAccess = fp + nBytesProcessed;
				survey->filePositionAfterLastSeek = survey->filePositionAfterLastAccess;
			}
			else
			{
				// Empty payload data?
				ERROR( "No size of payload data attribute found in activity " << activity->aid() );
			}

			// Update statistics
			survey->nAccessesWrite++;
			survey->timeTotalWrite += activity->time_stop() - activity->time_start();
		} // collectionLock

		// OUTPUT( "openSurveys size = " << openFileSurveys.size() );
		// OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
	}
}


/*
 * Close survey object, finalize its statistics and move it to list of closed surveys
 */
void FileSurveyorPlugin::closeSurvey( const shared_ptr<Activity> & activity, int lost  )
{

	if( !tryEnsureInitialization() ) return;

	// OUTPUT( "closeSurvey() received activity " << activity->aid() << ": " << sysinfo->lookup_activity_name( activity->ucaid()) );

	const ActivityID *	parentAID = findParentAID( activity );
	//OUTPUT( "Parent Array Size = " << activity->parentArray().size() );
	//OUTPUT( "Parent AID * = " << parentAID );

	if( parentAID != NULL )
	{
		FileSurvey survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			try {
				survey = openFileSurveys.at( *parentAID );
			}
			catch( NotFoundError ) {
				// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost to // SIOX, or when the file name did not match the pattern given in the configuration.
				// Therefore, disregard all following activities on those files until closed and re-opened.
				//ERROR( "No parent activity found for activity " << activity->aid() );
				return;
			}

			// Update statistics
			survey.nCloses++;
			survey.timeClosed = activity->time_stop();
			survey.timeTotalClose += activity->time_stop() - activity->time_start();

			// Move survey from open to closed collection
			openFileSurveys.erase( *parentAID );
			closedFileSurveys.push_back( survey );
			// OUTPUT( "openSurveys size = " << openFileSurveys.size() );
			// OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
		} // collectionLock
	}
}


ComponentReport FileSurveyorPlugin::prepareReport()
{
	ComponentReport result;

	if( !tryEnsureInitialization() ) return ComponentReport();

	{	// Disallow other access to our survey collections
		lock_guard<mutex> collectionLock( collectionMutex );

		if( individualReports )
			for( auto survey : closedFileSurveys )
				reportSurvey(result, survey);

		if( aggregatedReport )
			reportSurvey(result, aggregateSurveys());

	} // collectionLock

	return result;
}

void FileSurveyorPlugin::reportSurvey(ComponentReport & report, const FileSurvey & survey)
{
	// uint64_t nAccessesReadRandom = survey.nAccessesReadRandomShort + survey.nAccessesReadRandomLong;
	// uint64_t nAccessesRead = nAccessesReadRandom + survey.nAccessesReadSequential;
	// uint64_t nAccessesWriteRandom = survey.nAccessesWriteRandomShort + survey.nAccessesWriteRandomLong;
	// uint64_t nAccessesWrite = nAccessesWriteRandom + survey.nAccessesWriteSequential;
	uint64_t nManagement = survey.nOpens + survey.nSeeks + survey.nCloses;
	uint64_t nAccesses = survey.nAccessesRead + survey.nAccessesWrite;
	uint64_t nBytes = survey.nBytesRead + survey.nBytesWrite;

	Timestamp timeTotalAccess = survey.timeTotalRead + survey.timeTotalWrite;
	Timestamp timeTotalManage = survey.timeTotalSeek + survey.timeTotalOpen + survey.timeTotalClose;
	Timestamp timeTotalSurvey = survey.timeClosed - survey.timeFirstOpened;

	// TODO: Rather base average seek distance on accesses actually seeking?
	double nBytesReadAverage = 0.0;
	double nSeekDistanceReadAverage = 0.0;
	if( survey.nAccessesRead > 0 ){
		nBytesReadAverage = survey.nBytesRead / ((double) survey.nAccessesRead);
		nSeekDistanceReadAverage = survey.nTotalSeekDistanceRead / ((double) survey.nAccessesRead);
	}
	double nBytesWriteAverage = 0.0;
	double nSeekDistanceWriteAverage = 0.0;
	if( survey.nAccessesWrite > 0 ){
		nBytesWriteAverage = ((double) survey.nBytesWrite) / ((double) survey.nAccessesWrite);
		nSeekDistanceWriteAverage = survey.nTotalSeekDistanceWrite / ((double) survey.nAccessesWrite);
	}


	GroupEntry * geFile = new GroupEntry( survey.fileName );

	// General
	// Does not make any sense to include the USER ID as this is the same for all...
	//report.addEntry( new GroupEntry( "User ID", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.userID ) ));
	//report.addEntry( new GroupEntry( "Time opened", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeOpened ) ));
	//report.addEntry( new GroupEntry( "Time closed", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeClosed ) ));

	// # Calls
	// Management
	GroupEntry * geManagement = new GroupEntry( "Management", geFile );
	report.addEntry( geManagement, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nManagement ) ));
		// Opening
		GroupEntry * geOpening = new GroupEntry( "Opening", geManagement );
		report.addEntry( new GroupEntry( "Opens, total", geOpening ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nOpens ) ));
		// Closing
		GroupEntry * geClosing = new GroupEntry( "Closing", geManagement );
		report.addEntry( new GroupEntry( "Closes, total", geClosing ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nCloses ) ));
		// Seeking
		GroupEntry * geSeeking = new GroupEntry( "Seeking", geManagement );
		report.addEntry( new GroupEntry( "Seeks, total", geSeeking ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nSeeks ) ));
		report.addEntry( new GroupEntry( "Seeks, short", geSeeking ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nSeeksShort ) ));
		report.addEntry( new GroupEntry( "Seeks, long", geSeeking ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nSeeksLong ) ));
	// Accesses
	GroupEntry * geAccesses = new GroupEntry( "Accesses", geFile );
	report.addEntry( geAccesses, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nAccesses ) ));
		// Reading
		GroupEntry * geReading = new GroupEntry( "Reading", geAccesses );
		report.addEntry( geReading, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesRead ) ));
		report.addEntry( new GroupEntry( "Sequential", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesReadSequential ) ));
		report.addEntry( new GroupEntry( "Random, short seek", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesReadRandomShort ) ));
		report.addEntry( new GroupEntry( "Random, long seek", geReading ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesReadRandomLong ) ));
		// Writing
		GroupEntry * geWriting = new GroupEntry( "Writing", geAccesses );
		report.addEntry( geWriting, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesWrite ) ));
		report.addEntry( new GroupEntry( "Sequential", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesWriteSequential ) ));
		report.addEntry( new GroupEntry( "Random, short seek", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesWriteRandomShort ) ));
		report.addEntry( new GroupEntry( "Random, long seek", geWriting ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nAccessesWriteRandomLong ) ));

	// # Bytes
	GroupEntry * geBytes = new GroupEntry( "Bytes", geFile );
	report.addEntry( geBytes, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytes ) ));
	report.addEntry( new GroupEntry( "Total read", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nBytesRead ) ));
	report.addEntry( new GroupEntry( "Read per access", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytesReadAverage ) ));
	report.addEntry( new GroupEntry( "Total written", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nBytesWrite ) ));
	report.addEntry( new GroupEntry( "Written per access", geBytes ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytesWriteAverage ) ));

	// # Time
	GroupEntry * geTime = new GroupEntry( "Time", geFile );
	report.addEntry( geBytes, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytes ) ));
	report.addEntry( new GroupEntry( "Total for opening (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalOpen ) ));
	report.addEntry( new GroupEntry( "Total for seeking (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalSeek ) ));
	report.addEntry( new GroupEntry( "Total for reading (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalRead ) ));
	report.addEntry( new GroupEntry( "Total for writing (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalWrite ) ));
	report.addEntry( new GroupEntry( "Total for closing (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalClose ) ));
	report.addEntry( new GroupEntry( "Total for accessing (read & write) (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalAccess ) ));
	report.addEntry( new GroupEntry( "Total for managing (seek, open & close) (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalManage ) ));
	report.addEntry( new GroupEntry( "Total surveyed (ns)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalSurvey ) ));

	// # Seek Distance
	GroupEntry * geSeekDistance = new GroupEntry( "Seek Distance", geFile );
	report.addEntry( geBytes, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytes ) ));
	report.addEntry( new GroupEntry( "Total", geSeekDistance ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nTotalSeekDistance ) ));
	report.addEntry( new GroupEntry( "Total, reading", geSeekDistance ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nTotalSeekDistanceRead ) ));
	report.addEntry( new GroupEntry( "Total, writing", geSeekDistance ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.nTotalSeekDistanceWrite ) ));
	report.addEntry( new GroupEntry( "Average reading", geSeekDistance ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nSeekDistanceReadAverage ) ));
	report.addEntry( new GroupEntry( "Average writing", geSeekDistance ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nSeekDistanceWriteAverage ) ));
}


bool FileSurveyorPlugin::tryEnsureInitialization()
{
	if( initLevel == initializedLevel ) return true;
	if( !initLevel ) return false;	//initPlugin() must be called from outside first!
	initPlugin();	//retry once we have our options
	return initLevel == initializedLevel;
}


/*
 * Find the UCAID for an activity of the interface we are to watch.
 * If no UCAID could be determined, will emit an error message and return 0.
 * Requires the UIID of the interface to be set!
 */
const UniqueComponentActivityID FileSurveyorPlugin::lookupUCAID( const string & activityName )
{
	UniqueComponentActivityID ucaid = 0;
	try{
		ucaid = sysinfo->lookup_activityID(uiid, activityName);
	}
	catch (NotFoundError)
	{
		ERROR( "Could not find ID for activity type \"" << activityName << "\"" );
	}

	return ucaid;
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
 const Attribute * FileSurveyorPlugin::findAttributeByID( const shared_ptr<Activity> & activity, OntologyAttributeID oaid )
 {
 	// OUTPUT( "Looking for attribute " << oaid << " in " << activity->aid() );

 	const vector<Attribute> & attributes = activity->attributeArray();
 	// OUTPUT( "Found att array of size " << attributes.size() );

	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		// OUTPUT( "...id: " << itr->id );
		if( itr->id == oaid )
			return &( *itr );
	}
	return NULL;
 }


/*
 * Aggregate statistics over all file surveys present
 */
const FileSurvey FileSurveyorPlugin::aggregateSurveys()
{
	FileSurvey total;

	{	// Disallow other access to our survey collections
		// lock_guard<mutex> collectionLock( collectionMutex );

		total.timeFirstOpened = -1;
		total.fileName = "(All files)";
		//total.userID = "(Aggregated over all users)";

		for(auto survey : closedFileSurveys){
			if (survey.timeFirstOpened < total.timeFirstOpened)
				total.timeFirstOpened = survey.timeFirstOpened;
			if (survey.timeClosed > total.timeClosed)
				total.timeClosed = survey.timeClosed;
			total.timeTotalOpen += survey.timeTotalOpen;
			total.timeTotalSeek += survey.timeTotalSeek;
			total.timeTotalRead += survey.timeTotalRead;
			total.timeTotalWrite += survey.timeTotalWrite;
			total.timeTotalClose += survey.timeTotalClose;
			total.nAccessesRead += survey.nAccessesRead;
			total.nAccessesReadSequential += survey.nAccessesReadSequential;
			total.nAccessesReadRandomShort += survey.nAccessesReadRandomShort;
			total.nAccessesReadRandomLong += survey.nAccessesReadRandomLong;
			total.nAccessesWrite += survey.nAccessesWrite;
			total.nAccessesWriteSequential += survey.nAccessesWriteSequential;
			total.nAccessesWriteRandomShort += survey.nAccessesWriteRandomShort;
			total.nAccessesWriteRandomLong += survey.nAccessesWriteRandomLong;
			total.nBytesRead += survey.nBytesRead;
			total.nBytesWrite += survey.nBytesWrite;
			total.nOpens += survey.nOpens;
			total.nCloses += survey.nCloses;
			total.nSeeks += survey.nSeeks;
			total.nSeeksShort += survey.nSeeksShort;
			total.nSeeksLong += survey.nSeeksLong;
			total.nTotalSeekDistance += survey.nTotalSeekDistance;
			total.nTotalSeekDistanceRead += survey.nTotalSeekDistanceRead;
			total.nTotalSeekDistanceWrite += survey.nTotalSeekDistanceWrite;
		}
	} // collectionLock

	return total;
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

void FileSurveyorPlugin::finalize() {
	multiplexer->unregisterCatchall( this, false );
	ActivityMultiplexerPlugin::finalize();
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileSurveyorPlugin();
	}
}
