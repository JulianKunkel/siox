#include <regex>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>

// for mkdir()
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <monitoring/ontology/Ontology.hpp>
#include <tools/TraceReader/plugin/CSVExtractor/CSVExtractor.hpp>


using namespace std;
using namespace monitoring;
using namespace tools;

#define INVALID_UINT64 ((uint64_t) -1)
#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}

void CSVExtractor::moduleOptions(program_options::options_description & od){
	od.add_options()
		( "file", program_options::value<string>()->default_value( "output.csv" ), "Name of the created CSV file" );
		// string regex = vm["plotFileGraphRegex"].as<string>();
}

void CSVExtractor::init(program_options::variables_map * vm, TraceReader * tr){
	o = tr->getOntology();

	addActivityHandler("POSIX", "", "open", & CSVExtractor::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "creat", & CSVExtractor::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "write", & CSVExtractor::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "read", & CSVExtractor::handlePOSIXRead);
	addActivityHandler("POSIX", "", "pwrite", & CSVExtractor::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "pread", & CSVExtractor::handlePOSIXRead);
	addActivityHandler("POSIX", "", "writev", & CSVExtractor::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "readv", & CSVExtractor::handlePOSIXRead);
	addActivityHandler("POSIX", "", "close", & CSVExtractor::handlePOSIXClose);
	addActivityHandler("POSIX", "", "sync", & CSVExtractor::handlePOSIXSync);
	addActivityHandler("POSIX", "", "fdatasync", & CSVExtractor::handlePOSIXSync);
	addActivityHandler("POSIX", "", "lseek", & CSVExtractor::handlePOSIXSeek);

	IGNORE_ERROR(fhID = o->lookup_attribute_by_name( "POSIX", "descriptor/filehandle" ).aID;)
	IGNORE_ERROR(fname = o->lookup_attribute_by_name( "POSIX", "descriptor/filename" ).aID;)
	IGNORE_ERROR(bytesReadID = o->lookup_attribute_by_name( "POSIX", "quantity/BytesRead" ).aID;)
	IGNORE_ERROR(positionID = o->lookup_attribute_by_name( "POSIX", "file/position" ).aID;)
	IGNORE_ERROR(bytesWrittenID = o->lookup_attribute_by_name( "POSIX", "quantity/BytesWritten" ).aID;)

	const string filename = (*vm)["file"].as<string>();
	cout << "Writing CSV file to " << filename << endl;
	csv.open(filename, ios::out | ios::trunc );
	csv << "DeltaTime,fd,OpTyp,DeltaOffset,Size,Duration" << endl;
}

void CSVExtractor::addActivityHandler(const string & interface, const string & impl, const string & a, void (CSVExtractor::* handler)(Activity*) )
{
	SystemInformationGlobalIDManager * s = tr->getSystemInformationGlobalIDManager();

	UniqueInterfaceID uiid = s->lookup_interfaceID( interface, impl );

	try{
		UniqueComponentActivityID  ucaid = s->lookup_activityID( uiid, a );
		activityHandlers[ucaid] = handler;
	}catch(NotFoundError & e){
		// we are not installing the handler if the type is unknown
	}
}

Activity * CSVExtractor::processNextActivity(Activity * a){
	if (tlast == 0){
		tlast = a->time_start_;
	}

	auto both = activityHandlers.find(a->ucaid_);

	if ( both != activityHandlers.end() ){
		auto fkt = both->second;

		if( verbosity > 3){
			tr->printActivity(a);
		}

		(this->*fkt)(a);
	}

	return a;
}

static double ttoDbl(Timestamp t){
	return t * 0.001 * 0.001 * 0.001;	
}

double CSVExtractor::convertUpdateTime(Timestamp t){
	Timestamp delta = t - this->tlast;
	this->tlast = t;

	return ttoDbl(delta);
}	

void CSVExtractor::finalize(){
	csv.close();
}

static const uint32_t findUINT32AttributeByID( const Activity * a, OntologyAttributeID oaid )
{
 	const vector<Attribute> & attributes = a->attributeArray();
	for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
		if( itr->id == oaid )
			return itr->value.uint32();
	}

	return 0;
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

OpenFiles * CSVExtractor::findParentFile( const Activity * a )
{
	const vector<ActivityID>& parents = a->parentArray();

	for( size_t i = 0; i < parents.size(); i++) {
		if( openFiles.count( parents[i] ) > 0 ){
			return & openFiles[ parents[i] ];
		}
	}

	return nullptr;
}

OpenFiles * CSVExtractor::findParentFileByFh( const Activity * a ){
	OpenFiles * parent = findParentFile(a);
	if ( parent == nullptr ){
		// add a dummy for the file handle since we do not know the filename
		uint32_t fh = findUINT32AttributeByID(a, fhID);

		if ( unnamedFiles.count(fh) > 0){
			parent = & unnamedFiles[fh];
		}else{
			stringstream s;
			s << fh;
			unnamedFiles[fh] = { "[UNKN]", 0 };
			parent = & unnamedFiles[fh];
		}
	}
	return parent;
}

void CSVExtractor::handlePOSIXSeek(Activity * a){
	OpenFiles * parent = findParentFileByFh(a);
	parent->currentPosition = findUINT64AttributeByID(a, positionID);
	// ",OpTyp,Size,DeltaOffset,Duration"
}

void CSVExtractor::handlePOSIXSync(Activity * a){
	int fd = findUINT32AttributeByID(a, fhID);
	Timestamp delta = a->time_stop_ - a->time_start_;
	csv << convertUpdateTime(a->time_start_) << "," << fd << "," << "S,0,0," << ttoDbl(delta) << endl;
}



void CSVExtractor::handlePOSIXAccess(Activity * a, string type){
	uint64_t bytes = findUINT64AttributeByID(a, bytesReadID);
	if (bytes == INVALID_UINT64){
		bytes = findUINT64AttributeByID(a, bytesWrittenID);	
	}	
	uint64_t position = findUINT64AttributeByID(a, positionID);
	uint64_t deltaPosition;

	//cout << bytes << ", " << position << endl;

	OpenFiles * parent = findParentFileByFh(a);
	if ( position == INVALID_UINT64){
		position = parent->currentPosition;
		parent->currentPosition += bytes;
	}

	// position = 1000, lastAccessPosition = 500
	deltaPosition = position - parent->lastAccessPosition;
	parent->lastAccessPosition = parent->currentPosition;
	int fd = findUINT32AttributeByID(a, fhID);
	Timestamp delta = a->time_stop_ - a->time_start_;
	csv << convertUpdateTime(a->time_start_) << "," << fd << "," << type << "," << deltaPosition << "," << bytes << "," << ttoDbl(delta) << endl;
}


void CSVExtractor::handlePOSIXWrite(Activity * a){
	handlePOSIXAccess(a, "W");
}

void CSVExtractor::handlePOSIXRead(Activity * a){
	handlePOSIXAccess(a, "R");
}

void CSVExtractor::handlePOSIXOpen(Activity * a){
	openFiles[ a->aid() ] = { findStrAttributeByID(a, fname), 0};
}

void CSVExtractor::handlePOSIXClose(Activity * a){
	OpenFiles * parent = findParentFileByFh(a);
}

extern "C" {
	void * TOOLS_TRACE_READER_INSTANCIATOR_NAME()
	{
		return new CSVExtractor();
	}
}