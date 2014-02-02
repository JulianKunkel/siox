/**
 
 This POSIX optimization plugin tracks accesses for each file-descriptor.
 It measures the time for each read/write/pwrite/pread/readv/writev,
 predicts the next access position and issues a posix_fadvise(POSIX_FADV_WILLNEED) to the next offsets it will need.
 Further it has to track open/creat to capture the filename and lseek.

 The plugin will manage a statistics about correctly predicted and miss-predicted regions.
 It will only issue fadvise() if a series of at last 4 access is correctly predicted.
 A write to a region that should be prefetched will invalidate a prediction as well (as we try to prefetch data that is overwritten in the meantime).

 The plugin supports strided access, thus if I access 0, 50, 100 it will predict the next offset to be 150.

 At completion it will output the collected statistics.

 * @author JK
 */
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <cmath>
#include <mutex>

#include <core/reporting/ComponentReportInterface.hpp>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>

#include <monitoring/topology/Topology.hpp>

#include <monitoring/activity_multiplexer/plugins/fadviseReadAhead/fadviseReadAheadOptions.hpp>


using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;


#define OUTPUT(...) do { cout << "[fadviseReadAhead] " << __VA_ARGS__ << "\n"; } while(0)
//#define OUTPUT(...) 

#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}
#define INVALID_UINT64 ((uint64_t) -1)


struct FileStatistics{
	FileStatistics(){}
	FileStatistics(string filename) : filename(filename){}

	string filename; 

	uint64_t correctPredictions = 0;
	uint64_t missPredictions = 0;
	uint64_t prefetchesConducted = 0;
	uint64_t prefetchedData = 0;

	// any missprediction will reset the following counter to zero
	uint64_t correctPredictionInARow = 0;

	// the prediction for the next access
	uint64_t nextPredictionSize = 0;
	uint64_t nextPredictionOffset = 0;

	// the last offset, size accessed (needed to predict the stride)
	uint64_t lastReadOffset = 0;
	uint64_t lastReadSize = 0;
};

static const uint32_t findUINT32AttributeByID( const Activity * a, OntologyAttributeID oaid )
{	
 	const vector<Attribute> & attributes = a->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {

		if( itr->id == oaid ){
			return itr->value.uint32();
		}
	}

	return (uint32_t) - 1;
}

static const uint64_t findUINT64AttributeByID( const Activity * a, OntologyAttributeID oaid )
{
 	const vector<Attribute> & attributes = a->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if( itr->id == oaid )
			return itr->value.uint64();
	}

	return INVALID_UINT64;
}

static const char * findStrAttributeByID( const Activity * a, OntologyAttributeID oaid )
{
 	const vector<Attribute> & attributes = a->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if( itr->id == oaid )
			return itr->value.str();
	}

	return "unknown";
}

class FadviseReadAheadPlugin: public ActivityMultiplexerPlugin, public ComponentReportInterface, public AnomalyPlugin {
	public:
		void initPlugin() override;		
		void Notify( const shared_ptr<Activity> & activity ) override;
		ComponentReport prepareReport() override;

		ComponentOptions * AvailableOptions() override;
	private:
		mutex giant_mutex;
		
		// statistics by parent file_open
		unordered_map<ActivityID, FileStatistics> statistics;
		unordered_map<int, FileStatistics> statisticsUnnamedFiles;

		unordered_map<UniqueComponentActivityID, void (FadviseReadAheadPlugin::*)(Activity*)> activityHandlers;

		FileStatistics * findParentFile( const Activity * activity );
		FileStatistics * findParentFileByFh( const Activity * activity );

		void handlePOSIXOpen(Activity * activity);
		void handlePOSIXWrite(Activity * activity);
		void handlePOSIXRead(Activity * activity);
		void handlePOSIXClose(Activity * activity);

		void addReport(ComponentReport & report, FileStatistics & statistics, GroupEntry * ge);

		void addActivityHandler(ActivityPluginDereferencing * f, const string & interface, const string & impl, const string & a, void (FadviseReadAheadPlugin::* handler)(Activity*) );
		
		// Ontology attributes needed in the plugin
		OntologyAttributeID fhID;
		OntologyAttributeID fname;
		OntologyAttributeID bytesReadID;
		OntologyAttributeID positionID;
		OntologyAttributeID bytesWrittenID;
};


FileStatistics * FadviseReadAheadPlugin::findParentFile( const Activity * a )
{
	const vector<ActivityID>& parents = a->parentArray();

	for( size_t i = 0; i < parents.size(); i++) {
		if( statistics.count( parents[i] ) > 0 ){
			return & statistics[ parents[i] ];
		}
	}

	return nullptr;
}

FileStatistics * FadviseReadAheadPlugin::findParentFileByFh( const Activity * a ){
	FileStatistics * parent = findParentFile(a);
	if ( parent == nullptr ){
		// add a dummy for the file handle since we do not know the filename
		uint32_t fh = findUINT32AttributeByID(a, fhID);

		if ( statisticsUnnamedFiles.count(fh) > 0){
			parent = & statisticsUnnamedFiles[fh];
		}else{
			stringstream s;
			s << fh;
			statisticsUnnamedFiles[fh] = { s.str() };
			parent = & statisticsUnnamedFiles[fh];
		}
	}
	return parent;
}



ComponentOptions * FadviseReadAheadPlugin::AvailableOptions() {
  return new FadviseReadAheadPluginOptions();
}


void FadviseReadAheadPlugin::initPlugin() {
	assert( facade != nullptr );

	FadviseReadAheadPluginOptions & o = getOptions<FadviseReadAheadPluginOptions>();
	ActivityPluginDereferencing * f = GET_INSTANCE(ActivityPluginDereferencing, o.dereferenceFacade);

	try{
		addActivityHandler(f, "POSIX", "", "open", & FadviseReadAheadPlugin::handlePOSIXOpen);
		addActivityHandler(f, "POSIX", "", "creat", & FadviseReadAheadPlugin::handlePOSIXOpen);
		addActivityHandler(f, "POSIX", "", "close", & FadviseReadAheadPlugin::handlePOSIXClose);
		addActivityHandler(f, "POSIX", "", "write", & FadviseReadAheadPlugin::handlePOSIXWrite);
		addActivityHandler(f, "POSIX", "", "read", & FadviseReadAheadPlugin::handlePOSIXRead);
		addActivityHandler(f, "POSIX", "", "pwrite", & FadviseReadAheadPlugin::handlePOSIXWrite);
		addActivityHandler(f, "POSIX", "", "pread", & FadviseReadAheadPlugin::handlePOSIXRead);
		addActivityHandler(f, "POSIX", "", "writev", & FadviseReadAheadPlugin::handlePOSIXWrite);
		addActivityHandler(f, "POSIX", "", "readv", & FadviseReadAheadPlugin::handlePOSIXRead);

	//We will use lseek to determine the file position if needed manually!
	//addActivityHandler(f, "POSIX", "", "lseek", & FadviseReadAheadPlugin::handlePOSIXSeek);

		fhID = f->lookup_attribute_by_name( "POSIX", "descriptor/filehandle" ).aID;
		fname = f->lookup_attribute_by_name( "POSIX", "descriptor/filename" ).aID;	
		positionID = f->lookup_attribute_by_name( "POSIX", "file/position" ).aID;
		bytesReadID = f->lookup_attribute_by_name( "POSIX", "quantity/BytesRead" ).aID;	
		bytesWrittenID = f->lookup_attribute_by_name( "POSIX", "quantity/BytesWritten" ).aID;
	}catch(const NotFoundError & e){
		cerr << "Warning, disabling FadviseReadAheadPlugin because some attributes could not be fetched. error: " << e.what() << endl;
	}
}


void FadviseReadAheadPlugin::handlePOSIXWrite(Activity * a){	
	uint64_t bytes = findUINT64AttributeByID(a, bytesWrittenID);
	uint64_t position = findUINT64AttributeByID(a, positionID);

	unique_lock<mutex> lock( giant_mutex );
	FileStatistics * fs = findParentFileByFh(a);

	uint64_t realPosition = position;
	if ( position == INVALID_UINT64){
		uint32_t fh = findUINT32AttributeByID(a, fhID);
		realPosition = lseek(fh, 0, SEEK_CUR);
	}

	// check if the write region overlaps with the predicted read-region
	if ( (realPosition + bytes) > fs->nextPredictionOffset && (realPosition < (fs->nextPredictionSize + fs->nextPredictionOffset) ) ){
		// it overlaps => reset our prediction
		fs->correctPredictionInARow = 0;
	}
}

void FadviseReadAheadPlugin::handlePOSIXRead(Activity * a){	
	uint64_t bytes = findUINT64AttributeByID(a, bytesReadID);
	uint32_t fh = findUINT32AttributeByID(a, fhID);
	uint64_t position = findUINT64AttributeByID(a, positionID);

	unique_lock<mutex> lock( giant_mutex );
	FileStatistics * fs = findParentFileByFh(a);

	uint64_t realPosition = position;
	if ( position == INVALID_UINT64){		
		realPosition = lseek(fh, 0, SEEK_CUR);
	}	

	// check if the read is EXACTLY the predicted region to read-ahead
	if ( realPosition == fs->nextPredictionOffset && bytes == fs->nextPredictionSize ){
		// perfect prediction
		fs->correctPredictionInARow++;
		fs->correctPredictions++;
	}else{
		fs->correctPredictionInARow = 0;
		fs->missPredictions++;
	}

	// predict the next region using the same stride.
	fs->nextPredictionSize = bytes;
	fs->nextPredictionOffset = realPosition + (realPosition - fs->lastReadOffset);


	if ( fs->correctPredictionInARow > 4 ){
		// prefetching
		int ret = posix_fadvise(fh, fs->nextPredictionOffset, fs->nextPredictionSize, POSIX_FADV_WILLNEED);
		if ( ret == 0 ){
			fs->prefetchesConducted++;
			fs->prefetchedData += bytes;
		}else{
			OUTPUT( fs->correctPredictionInARow << " pos: " << realPosition << " nextPos: " << fs->nextPredictionOffset << " fadvise " << fh << " " <<  strerror(ret) );
		}
	}

	// remember the last region
	fs->lastReadOffset = realPosition;
	fs->lastReadSize = bytes;
}

void FadviseReadAheadPlugin::handlePOSIXOpen(Activity * a){
	unique_lock<mutex> lock( giant_mutex );
	statistics[ a->aid() ] = { findStrAttributeByID(a, fname) };
}

void FadviseReadAheadPlugin::handlePOSIXClose(Activity * a){
	// unique_lock<mutex> lock( giant_mutex );
	FileStatistics * parent = findParentFileByFh(a);
	// TODO we could remove the statistics theoretically. 
	// Shall we remember states between different open's
}


void FadviseReadAheadPlugin::addActivityHandler(ActivityPluginDereferencing * f, const string & interface, const string & impl, const string & a, void (FadviseReadAheadPlugin::* handler)(Activity*) )
{
	SystemInformationGlobalIDManager * s = f->get_system_information();

	UniqueInterfaceID uiid = s->lookup_interfaceID( interface, impl );	
	
	try{
		UniqueComponentActivityID  ucaid = s->lookup_activityID( uiid, a );
		activityHandlers[ucaid] = handler;
	}catch(NotFoundError & e){
		// we are not installing the handler if the type is unknown
	}
}

void FadviseReadAheadPlugin::Notify( const shared_ptr<Activity> & activity ) {
	auto both = activityHandlers.find(activity->ucaid_);

	if ( both != activityHandlers.end() ){
		auto fkt = both->second;
		(this->*fkt)(& * activity);
	}
}

void FadviseReadAheadPlugin::addReport(ComponentReport & report, FileStatistics & fs, GroupEntry * ge){
		report.addEntry( new GroupEntry( "prefetchedData", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( fs.prefetchedData ) ));
		report.addEntry( new GroupEntry( "correctPredictions", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( fs.correctPredictions ) ));
		report.addEntry( new GroupEntry( "missPredictions", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( fs.missPredictions ) ));
		report.addEntry( new GroupEntry( "prefetchesDone", ge ), ReportEntry( ReportEntry::Type::APPLICATION_INFO, VariableDatatype( fs.prefetchesConducted ) ));		
}

ComponentReport FadviseReadAheadPlugin::prepareReport()
{
	ComponentReport report;

	for( auto itr = statistics.begin(); itr != statistics.end(); itr++ ){
		FileStatistics & fs = itr->second;

		GroupEntry * ge = new GroupEntry(fs.filename);
		addReport(report, fs, ge);

	}

	for( auto itr = statisticsUnnamedFiles.begin(); itr != statisticsUnnamedFiles.end(); itr++ ){
		FileStatistics & fs = itr->second;

		stringstream name;
		name << "fd:" << itr->first;

		GroupEntry * ge = new GroupEntry(name.str());
		addReport(report, fs, ge);
	}

	return report;
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FadviseReadAheadPlugin();
	}
}
