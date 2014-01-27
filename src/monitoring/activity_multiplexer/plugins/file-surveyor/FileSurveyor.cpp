/**
 * An ActivityMultiplexerPlugin to watch the stream of incoming activites,
 * group information on the ones pertaining to the same (user, file) combination
 * and report them upon component shutdown.
 * It can be applied to any layer that supports the semantics of OPEN, READ, WRITE, SEEK and CLOSE.
 * A configuration file defines the symbols which map to the OPEN etc. semantics
 * and an integer limit to differentiate between short and long SEEKs.
 *
 * @author Michaela Zimmer, Nathanael Hübbe, Julian Kunkel
 * @date 2014-01-27
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
		string userID = "[NO VALID UID]";
		// Counters for current open-access-close cycle
		Timestamp timeFirstOpened = 0;
		Timestamp timeOpened = 0;
		Timestamp timeClosed = 0;
		uint64_t filePosition = 0;
		// Aggregated statistics over all cycles
		Timestamp timeTotalOpen = 0;
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
		uint64_t nTotalSeekDistanceRead = 0;
		uint64_t nTotalSeekDistanceWrite = 0;
};

// TODO: Aggregate over all files
// TODO: Total access time

class FileSurveyorPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface {
	public:
		// FileSurveyorPlugin() { }

		void initPlugin() override;
		ComponentOptions * AvailableOptions() override;

		void Notify( shared_ptr<Activity> activity ) override;

		virtual ComponentReport prepareReport() override;

		//~FileSurveyorPlugin() { }

	private:
		int initLevel = 0;
		const int initializedLevel = -1;

		SystemInformationGlobalIDManager * sysinfo;

		// Information about the layer we are to watch
		string interface;
		string implementation;
		UniqueInterfaceID uiid;

		// OAIDs of various attributes
		OntologyAttributeID uidAttID;	// user-id
		OntologyAttributeID fnAttID;	// file name
		OntologyAttributeID fpAttID;	// file position
		OntologyAttributeID btrAttID;	// bytes to read
		OntologyAttributeID btwAttID;	// bytes to write

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

		void openSurvey( shared_ptr<Activity> activity );
		void updateSurvey( shared_ptr<Activity> activity );
		void closeSurvey( shared_ptr<Activity> activity );

		const FileSurvey aggregateSurveys();
		void reportSurvey(ComponentReport & report, const FileSurvey & survey);

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
			initLevel = 1;	//Once we are called from outside, there must exist sensible options for us, so if initialization fails now, we can simply retry.

		case 1:
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
			// Fill the token maps with every known activity's UCAID and type
			// First: OPEN calls
			RETURN_ON_EXCEPTION(
				for( auto token : o.openTokens )
					types[sysinfo->lookup_activityID(uiid,token)] = OPEN;
			);
			initLevel = 3;

		case 3:
			// Second: READ calls
			RETURN_ON_EXCEPTION(
				for( auto token : o.readTokens )
					types[sysinfo->lookup_activityID(uiid,token)] = READ;
			);
			initLevel = 4;

		case 4:
			// Third: WRITE calls
			RETURN_ON_EXCEPTION(
				for( auto token : o.writeTokens )
					types[sysinfo->lookup_activityID(uiid,token)] = WRITE;
			);
			initLevel = 5;

		case 5:
			// Fourth: CLOSE calls
			RETURN_ON_EXCEPTION(
				for( auto token : o.closeTokens )
					types[sysinfo->lookup_activityID(uiid,token)] = CLOSE;
			);
			initLevel = 6;

		case 6:
			// Gather the list of file extensions we are to watch into a map (for easy reference)
			for( auto extension : o.fileExtensionsToWatch ){
				string ext = extension;
				toUpper(ext);
				fileExtensionsToWatch.emplace(ext);
			}
			initLevel = 7;

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

		case READ:
		case WRITE: {
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
	string fileName = "(no valid FileName)";
	const Attribute * attFileName = findAttributeByID( activity, fnAttID );
	if( attFileName == NULL )
	{
		// This should not happen; it is, however, no serious problem; just don't do anything.
		cerr << "[FileSurvey] No valid file name in activity " << activity->aid() << "!" << endl;
		return;
	}
	fileName = attFileName->value.str();
	string fileExtension = findFileNameExtension(fileName);
	toUpper(fileExtension);
	//OUTPUT( "File extension found: " << fileExtension );
	if( fileExtensionsToWatch.count( fileExtension ) > 0 || fileExtensionsToWatch.empty() )
	{
		// Retrieve User ID
		string userID = "(no valid UserID)";
		// TODO: Retrieve correct UID for OPEN activities.
		// TODO: Add application name/path (and PID?) to tuple by which to differentiate surveys
		const Attribute * attUserID = findAttributeByID( activity, uidAttID );
		if ( attUserID == NULL ){
			cerr << "[FileSurvey] No valid user ID in activity" << activity->aid() << "!" << endl;
		}
		else{
			userID = attUserID->value.str();
			// OUTPUT( "UserID: " << userID );
		}

		FileSurvey	survey;

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			// See whether there is a survey for this user and file already that can be reopened
			for(auto candidate=closedFileSurveys.begin(); candidate != closedFileSurveys.end(); candidate++)
			{
				if( candidate->userID == attUserID->value.str()
				   && candidate->fileName == attFileName->value.str())
				{
					// Reuse closed survey

					// Update data
					survey = *candidate;
					survey.timeOpened = activity->time_start();
					survey.filePosition = 0;
					survey.timeTotalOpen += activity->time_stop() - activity->time_start();

					// Move survey back to open collection
					openFileSurveys[ activity->aid() ] = survey;
					closedFileSurveys.erase(candidate);

					return;
				}
			}

			// Create and fill new survey for the file just opened
			survey.timeFirstOpened = survey.timeOpened = activity->time_start();
			survey.fileName = attFileName->value.str();
			survey.userID = userID;
			survey.timeTotalOpen += activity->time_stop() - activity->time_start();

			openFileSurveys[ activity->aid() ] = survey;
		} // collectionLock
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

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

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

				// Update memorized file position
				survey->filePosition += nBytesProcessed;
				// Update access counts according to file position
				if( attFilePointer != NULL )
				{
					uint64_t fp = attFilePointer->value.uint64();
					uint64_t fpJump = absu64(fp, survey->filePosition);

					if( fpJump > 0 )
					{
						survey->filePosition = fp;
						if ( fpJump > seekSizeLimit )
							survey->nAccessesReadRandomLong++;
						else
							survey->nAccessesReadRandomShort++;
						survey->nTotalSeekDistanceRead += fpJump;
					}
					else
						survey->nAccessesReadSequential++;
					//OUTPUT( "file position = " << fp );
				}
				else
				{
					survey->nAccessesReadSequential++;
				}

				// Update statistics
				survey->nBytesRead += nBytesProcessed;
				survey->timeTotalRead += activity->time_stop() - activity->time_start();

				return;
			}

			// Second try: BytesToWrite
			attBytesProcessed = findAttributeByID( activity, btwAttID );
			if (attBytesProcessed != NULL )
			{
				uint64_t nBytesProcessed = attBytesProcessed->value.uint64();

				// Update memorized file position
				survey->filePosition += nBytesProcessed;
				// Update access counts according to file position
				if( attFilePointer != NULL )
				{
					uint64_t fp = attFilePointer->value.uint64();
					uint64_t fpJump = absu64(fp, survey->filePosition);

					if( fpJump > 0 )
					{
						survey->filePosition = fp;
						if ( fpJump > seekSizeLimit )
							survey->nAccessesWriteRandomLong++;
						else
							survey->nAccessesWriteRandomShort++;
						survey->nTotalSeekDistanceWrite += fpJump;
					}
					else
						survey->nAccessesWriteSequential++;
					//OUTPUT( "file position = " << fp );
				}
				else
				{
					survey->nAccessesWriteSequential++;
				}

				// Update statistics
				survey->nBytesWrite += nBytesProcessed;
				survey->timeTotalWrite += activity->time_stop() - activity->time_start();

				return;
			}
		} // collectionLock

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

		{	// Disallow other access to our survey collections
			lock_guard<mutex> collectionLock( collectionMutex );

			try {
				survey = openFileSurveys.at( *parentAID );
			}
			catch( NotFoundError ) {
				// May happen whenever SIOX "inherits" open files, e.g., ones whose open() calls were lost to // SIOX, or when the file name did not match the pattern given in the configuration.
				// Therefore, disregard all following activities on those files until closed and re-opened.
				//cerr << "[FileSurvey]: No parent activity found for activity " << activity->aid() << "!" << endl;
				return;
			}

			// Update statistics
			survey.timeClosed = activity->time_stop();
			survey.timeTotalClose += activity->time_stop() - activity->time_start();

			// Move survey from open to closed collection
			openFileSurveys.erase( *parentAID );
			closedFileSurveys.push_back( survey );
			OUTPUT( "openSurveys size = " << openFileSurveys.size() );
			OUTPUT( "closedSurveys size = " << closedFileSurveys.size() );
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
	uint64_t nAccessesReadRandom = survey.nAccessesReadRandomShort + survey.nAccessesReadRandomLong;
	uint64_t nAccessesRead = nAccessesReadRandom + survey.nAccessesReadSequential;
	uint64_t nAccessesWriteRandom = survey.nAccessesWriteRandomShort + survey.nAccessesWriteRandomLong;
	uint64_t nAccessesWrite = nAccessesWriteRandom + survey.nAccessesWriteSequential;
	uint64_t nAccesses = nAccessesRead + nAccessesWrite;
	uint64_t nBytes = survey.nBytesRead + survey.nBytesWrite;

	Timestamp timeTotalAccess = survey.timeTotalRead + survey.timeTotalWrite;
	Timestamp timeTotalManage = survey.timeTotalOpen + survey.timeTotalClose;
	Timestamp timeTotalSurvey = survey.timeClosed - survey.timeFirstOpened;

	// TODO: Rather base average seek distance on accesses actually seeking?
	double nBytesReadAverage = 0.0;
	double nSeekDistanceReadAverage = 0.0;
	if( nAccessesRead > 0 ){
		nBytesReadAverage = survey.nBytesRead / ((double) nAccessesRead);
		nSeekDistanceReadAverage = survey.nTotalSeekDistanceRead / ((double) nAccessesRead);
	}
	double nBytesWriteAverage = 0.0;
	double nSeekDistanceWriteAverage = 0.0;
	if( nAccessesWrite > 0 ){
		nBytesWriteAverage = ((double) survey.nBytesWrite) / ((double) nAccessesWrite);
		nSeekDistanceWriteAverage = survey.nTotalSeekDistanceWrite / ((double) nAccessesWrite);
	}


	GroupEntry * geFile = new GroupEntry( survey.fileName );

	// General
	// Does not make any sense to include the USER ID as this is the same for all...
	//report.addEntry( new GroupEntry( "User ID", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.userID ) ));
	//report.addEntry( new GroupEntry( "Time opened", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeOpened ) ));
	//report.addEntry( new GroupEntry( "Time closed", geFile ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeClosed ) ));

	// # Accesses
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
	report.addEntry( new GroupEntry( "Total for opening", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalOpen ) ));
	report.addEntry( new GroupEntry( "Total for reading", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalRead ) ));
	report.addEntry( new GroupEntry( "Total for writing", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalWrite ) ));
	report.addEntry( new GroupEntry( "Total for closing", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( survey.timeTotalClose ) ));
	report.addEntry( new GroupEntry( "Total for accessing (read & write)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalAccess ) ));
	report.addEntry( new GroupEntry( "Total for managing (open & close)", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalManage ) ));
	report.addEntry( new GroupEntry( "Total surveyed", geTime ), ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( timeTotalSurvey ) ));

	// # Seek Distance
	GroupEntry * geSeekDistance = new GroupEntry( "Seek Distance", geFile );
	report.addEntry( geBytes, ReportEntry( ReportEntry::Type::SIOX_INTERNAL_INFO, VariableDatatype( nBytes ) ));
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
 * Aggregate statistics over all file surveys present
 */
const FileSurvey FileSurveyorPlugin::aggregateSurveys()
{
	FileSurvey total;

	{	// Disallow other access to our survey collections
		// lock_guard<mutex> collectionLock( collectionMutex );

		total.timeFirstOpened = -1;

		for(auto survey : closedFileSurveys){

			total.fileName = "(Aggregated over all files)";
			total.userID = "(Aggregated over all users)";
			if (survey.timeFirstOpened < total.timeFirstOpened)
				total.timeFirstOpened = survey.timeFirstOpened;
			// timeOpened = 0;
			if (survey.timeClosed > total.timeClosed)
				total.timeClosed = survey.timeClosed;
			// filePosition = 0;
			total.timeTotalOpen += survey.timeTotalOpen;
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


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileSurveyorPlugin();
	}
}
