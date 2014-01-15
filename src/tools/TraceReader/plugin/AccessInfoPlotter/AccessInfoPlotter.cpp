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
#include <tools/TraceReader/plugin/AccessInfoPlotter/AccessInfoPlotter.hpp>


using namespace std;
using namespace monitoring;
using namespace tools;

#define INVALID_UINT64 ((uint64_t) -1)
#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}

void AccessInfoPlotter::moduleOptions(program_options::options_description & od){
	od.add_options()
		( "plotFileGraphRegex", program_options::value<string>()->default_value( ".*" ), "Extract for each file matching the regular expression the time/offset tupels for plotting. The regular expression for which access information is extracted" );
		// string regex = vm["plotFileGraphRegex"].as<string>();
}

void AccessInfoPlotter::init(program_options::variables_map * vm, TraceReader * tr){
	//vector<string> supportedOpens("MPI_File_open", 	"open", "creat");
	//vector<string> supportedClose("MPI_File_close", "close");
	//vector<string> supportedAccess("read", "write", "pread", "pwrite");

	o = tr->getOntology();

	addActivityHandler("POSIX", "", "open", & AccessInfoPlotter::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "creat", & AccessInfoPlotter::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "write", & AccessInfoPlotter::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "read", & AccessInfoPlotter::handlePOSIXRead);
	addActivityHandler("POSIX", "", "pwrite", & AccessInfoPlotter::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "pread", & AccessInfoPlotter::handlePOSIXRead);	
	addActivityHandler("POSIX", "", "writev", & AccessInfoPlotter::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "readv", & AccessInfoPlotter::handlePOSIXRead);
	addActivityHandler("POSIX", "", "close", & AccessInfoPlotter::handlePOSIXClose);
	addActivityHandler("POSIX", "", "sync", & AccessInfoPlotter::handlePOSIXSync);
	addActivityHandler("POSIX", "", "fdatasync", & AccessInfoPlotter::handlePOSIXSync);
	addActivityHandler("POSIX", "", "lseek", & AccessInfoPlotter::handlePOSIXSeek);

	IGNORE_ERROR(fhID = o->lookup_attribute_by_name( "POSIX", "descriptor/filehandle" ).aID;)
	IGNORE_ERROR(fname = o->lookup_attribute_by_name( "POSIX", "descriptor/filename" ).aID;)
	IGNORE_ERROR(bytesReadID = o->lookup_attribute_by_name( "POSIX", "quantity/BytesRead" ).aID;)
	IGNORE_ERROR(positionID = o->lookup_attribute_by_name( "POSIX", "file/position" ).aID;)
	IGNORE_ERROR(bytesWrittenID = o->lookup_attribute_by_name( "POSIX", "quantity/BytesWritten" ).aID;)
}

void AccessInfoPlotter::addActivityHandler(const string & interface, const string & impl, const string & a, void (AccessInfoPlotter::* handler)(Activity*) )
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

Activity * AccessInfoPlotter::processNextActivity(Activity * a){
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

static double convertTime(Timestamp time){
	return time * 0.001 * 0.001 * 0.001;
}

void AccessInfoPlotter::plotSingleFileAccess(const string & type, const vector<Access> & accessVector, ofstream & f, const OpenFiles & file){
	if ( accessVector.size() == 0 ){
		f << type << "X = []" << endl;
		f << type << "Y = []" << endl;
		return;
	}
	if( verbosity > 2){
		cout << "File: " << file.name << " " << type << endl;
		for( auto tupel = accessVector.begin(); tupel != accessVector.end(); tupel++ ){
			cout << (tupel->startTime - file.openTime) << "-"  << (tupel->endTime - file.openTime) << " " << tupel->offset << "," << tupel->size << endl;
		}
	}
	// add times
	f << type << "X = [[" ;
	f << convertTime(accessVector.begin()->startTime - file.openTime);
	for( auto tupel = ++accessVector.begin(); tupel != accessVector.end(); tupel++ ){
		f  << "," << convertTime(tupel->startTime - file.openTime);
	}
	f << "],[";
	f << convertTime(accessVector.begin()->endTime - file.openTime);
	for( auto tupel = ++accessVector.begin(); tupel != accessVector.end(); tupel++ ){
		f  << "," << convertTime(tupel->endTime - file.openTime);
	}	
	f << "]]" << endl;
	
	// values

	f << type << "Y = [[" ;
	f << (accessVector.begin()->offset);
	for( auto tupel = ++accessVector.begin(); tupel != accessVector.end(); tupel++ ){
		f  << ","  << tupel->offset;
	}	
	f << "],[";
	f << (accessVector.begin()->offset + accessVector.begin()->size);
	for( auto tupel = ++accessVector.begin(); tupel != accessVector.end(); tupel++ ){
		f  << "," << (tupel->offset + tupel->size);
	}
	f << "]]" << endl;
}

void AccessInfoPlotter::plotSingleFileOperation(const string & type, const vector<Operation> & ops, ofstream & f, const OpenFiles & file){	
	
	if ( ops.size() == 0 ){
		f << type << "X = []" << endl;
		f << type << "Y = []" << endl;
		return;
	}

	f << type << "X = [[" ;
	f << convertTime(ops.begin()->startTime - file.openTime);
	for( auto tupel = ++ops.begin(); tupel != ops.end(); tupel++ ){
		f  << "," << convertTime(tupel->startTime - file.openTime);
	}
	f << "],[";
	f << convertTime(ops.begin()->endTime - file.openTime);
	for( auto tupel = ++ops.begin(); tupel != ops.end(); tupel++ ){
		f  << "," << convertTime(tupel->endTime - file.openTime);
	}	
	f << "]]" << endl;
}

void AccessInfoPlotter::plotFileAccess(const OpenFiles & file){
	if (file.readAccesses.size() == 0 && file.writeAccesses.size() == 0){
		cout << "No accesses for file: " << file.name << endl;
		return;
	}

	stringstream nbuff;
	nbuff << file.name << "_" << file.aid ;
	string cleanedName = nbuff.str();
	replace( cleanedName.begin(), cleanedName.end(), '/', '_');
	replace( cleanedName.begin(), cleanedName.end(), ' ', '_');

	// create data file
	ofstream f;
	f.open ("plot/" + cleanedName + ".py");
	f << "title = \"" << file.name << '"' << endl;
	f << "filename = \"" << cleanedName << '"' << endl;
	f << "tOpen =  " << file.openTime << endl;
	f << "tClose =  " << file.closeTime << endl;
	
	// convert open time into a readable format
	time_t t = file.openTime / 1000 / 1000 / 1000;
	struct tm *tm = localtime(&t);
	char date[40];
	strftime(date, sizeof(date), "%Y-%m-%d %k:%M:%S", tm);
	f << "tOpenString = \"" << date << '"' << endl;

	plotSingleFileAccess("read", file.readAccesses, f, file);
	plotSingleFileAccess("write", file.writeAccesses, f, file);
	plotSingleFileOperation("sync", file.syncOperations, f, file);
	f.close();

	cout << "Run: siox-plot-trace-output \"plot/" + cleanedName << ".py\""  << endl;
}

void AccessInfoPlotter::finalize(){
	// create output directory
	mkdir("plot", 0777);

	for ( auto itr = openFiles.begin(); itr != openFiles.end(); itr++ ){
		plotFileAccess( itr->second );
	}
	for ( auto itr = unnamedFiles.begin(); itr != unnamedFiles.end(); itr++ ){
		plotFileAccess( itr->second );
	}
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

OpenFiles * AccessInfoPlotter::findParentFile( const Activity * a )
{
	const vector<ActivityID>& parents = a->parentArray();

	for( size_t i = 0; i < parents.size(); i++) {
		if( openFiles.count( parents[i] ) > 0 ){
			return & openFiles[ parents[i] ];
		}
	}

	return nullptr;
}

OpenFiles * AccessInfoPlotter::findParentFileByFh( const Activity * a ){
	OpenFiles * parent = findParentFile(a);
	if ( parent == nullptr ){
		// add a dummy for the file handle since we do not know the filename
		uint32_t fh = findUINT32AttributeByID(a, fhID);

		if ( unnamedFiles.count(fh) > 0){
			parent = & unnamedFiles[fh];
		}else{
			stringstream s;
			s << fh;
			unnamedFiles[fh] = { s.str(), a->time_start_, fh + 10000000 };
			parent = & unnamedFiles[fh];
		}
	}
	return parent;
}

void AccessInfoPlotter::handlePOSIXSeek(Activity * a){
	OpenFiles * parent = findParentFileByFh(a);
	parent->currentPosition = findUINT64AttributeByID(a, positionID);
}

void AccessInfoPlotter::handlePOSIXSync(Activity * a){
	OpenFiles * parent = findParentFileByFh(a);
	parent->syncOperations.push_back( {a->time_start_, a->time_stop_} );
}

void AccessInfoPlotter::handlePOSIXWrite(Activity * a){	
	uint64_t bytes = findUINT64AttributeByID(a, bytesWrittenID);
	uint64_t position = findUINT64AttributeByID(a, positionID);

	OpenFiles * parent = findParentFileByFh(a);
	uint64_t realPosition = position;
	if ( position == INVALID_UINT64){
		realPosition = parent->currentPosition;
		parent->currentPosition += bytes;
	}
	parent->writeAccesses.push_back( Access{a->time_start_, a->time_stop_, position, bytes} );
}

void AccessInfoPlotter::handlePOSIXRead(Activity * a){	
	uint64_t bytes = findUINT64AttributeByID(a, bytesReadID);
	uint64_t position = findUINT64AttributeByID(a, positionID);
	
	OpenFiles * parent = findParentFileByFh(a);
	uint64_t realPosition = position;
	if ( position == INVALID_UINT64){
		realPosition = parent->currentPosition;
		parent->currentPosition += bytes;
	}	
	parent->readAccesses.push_back( Access{a->time_start_, a->time_stop_, position, bytes} );
}

void AccessInfoPlotter::handlePOSIXOpen(Activity * a){
	openFiles[ a->aid() ] = { findStrAttributeByID(a, fname), a->time_start_, 0, 0, a->aid_};
}

void AccessInfoPlotter::handlePOSIXClose(Activity * a){
	OpenFiles * parent = findParentFileByFh(a);
	parent->closeTime = a->time_stop_;
}


/*
void test(){
			regex fileRegex(regex);

			Ontology * o;

}
*/

extern "C" {
	void * TOOLS_TRACE_READER_INSTANCIATOR_NAME()
	{
		return new AccessInfoPlotter();
	}
}