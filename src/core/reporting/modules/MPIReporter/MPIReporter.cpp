#include <mpi.h>

#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream> 
#include <unordered_map>

#include <core/reporting/modules/ConsoleReporter/ConsoleReporter.hpp>

#include "MPIReporterOptions.hpp"

using namespace std;
using namespace core;

/*
 The MPIReporter gathers the individual reports of all participating ranks, aggregates them and 
 outputs Average, Min & Max for each interesting metric.


 Since every process may have different Entries which must be aggregated
 it is difficult to use MPI_*Reduce().
 
 Questions:
 Q1) How can we treat processes that do not have a reportEntry?
 If we use MPI_Reduce(), then the problem is that we have to define values for all processes.
 A value of 0 is inappropriate for times, yet correct for invocation counts...
 

 Aggregation algorithm:
 Alternative 1)
	 First every process prepares a buffer with all entry names.
	 Then use MPI_Gather(buffer_len_vector) to communicate the buffer size of each rank to rank 0.
	 With MPI_Gatherv(buffer_len_vector) all names are send to rank 0.
	 All entry values are converted to doubles.
	 With MPI_Gatherv(buffer_len_vector) all values are send to rank 0.

	 Rank 0 aggregates all data ...

 Alternative 2)
 	A process sends a message to rank 0 which contains the number of entries and the size of the compact group names.
 	The same process sends the entry names and the values.

 	Rank 0 waits for an arbitrary message.
 	Processes it and waits for a specific message from the sender.

 The algorithm could be replaced with a binary tree algorithm some time...

 */
class MPIReporter : public ConsoleReporter{
private:
	struct Entry{
		double sum = 0;
		double min = 1e308;
		double max = 0;

		int processes = 0;

		double average() const{
			return sum / processes;
		}

		void addValue(double value){
			processes++;
			sum += value;
			min = min < value ? min : value;
			max = max > value ? max : value;
		}
	};

	struct EntryGroup{
		string moduleName;
		string sectionName;
		int componentID;
		unordered_map<string, Entry> map;
	};	

public:

	bool shouldEntryBeFiltered(const ReportEntry & entry);

	void packGroupNames(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports, string & msgData, uint & countEntries);
	void printAggregatedResult(ostream & out, unordered_map<string, EntryGroup> & aggregatedResult);

	void processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports);

	void processFinalReport(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		char * outputFilename = getenv("SIOX_MPI_REPORTING_FILENAME");
		processReport(outputFilename, reports);
	}

	virtual ComponentOptions* AvailableOptions(){
		return new MPIReporterOptions();
	}

   virtual void init(){
		char * priorityStr = getenv("SIOX_MPI_REPORTING_PRIORITY");
		if ( priorityStr ){
			minPriority = atoi(priorityStr) ;
		}
   }

private:

  	uint8_t minPriority = 0;

  	/*
  	Process the data received from a single process and aggregate the values to the current set.
  	*/
	void processMessage( int rank, uint stringLen, uint groupEntries, const char * msgData, const double * values, unordered_map<string, EntryGroup> & map );
};


void MPIReporter::processMessage( int rank, uint stringLen, uint groupEntries, const char * msgData, const double * values, unordered_map<string, EntryGroup> & map ){

	// cout << rank << ":" << stringLen << " " << groupEntries << endl;

	const char * endPos = stringLen + msgData;
	const char * curPos = msgData;

	const double * valuePointer = values;

	while( curPos < endPos ){

		const char * moduleName = curPos;
		curPos += strlen(moduleName) + 1;  		

		// parse section name
		const char * sectionName = curPos;
		curPos += strlen(sectionName) + 1;  				

		// parse component ID  				
		const char * componentID = curPos;
		curPos += strlen(componentID) + 1;

		string groupName = string(sectionName) + string(moduleName) + string(componentID);

		EntryGroup * entryGroup;
		unordered_map<string, EntryGroup>::iterator pos;
		if ( (pos = map.find(groupName)) != map.end() ){
			entryGroup = & pos->second;
		}else{
			map[groupName] = EntryGroup();
		entryGroup = & map.find(groupName)->second;
		entryGroup->sectionName = sectionName;
		entryGroup->moduleName = moduleName;
		entryGroup->componentID = atoi(componentID);
		}

		// parse report entry name, loop until \0\0 is detected which closes the group
		while( *curPos != '\0' ){
			const char * entryName = curPos;
			Entry & entry = entryGroup->map[entryName];
			entry.addValue(*valuePointer);

			valuePointer++;

			//cout << entryName << endl;
			curPos += strlen(entryName) + 1;
		}
		// check if we reached the end of the message.
		// skip the last \0
		curPos ++;
	}
}

void MPIReporter::packGroupNames(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports, string & msgData, uint & countEntries){
	countEntries = 0;
	// detect sizes and pack messages together
	stringstream messageData;
	for ( auto itr = reports.begin(); itr != reports.end(); itr++ ){
		RegisteredComponent* rc = itr->first;
		messageData << rc->moduleName << '\0' << rc->section << '\0' << (rc->id > 1000000? 0 : rc->id) << '\0';

		const ComponentReport & creport = itr->second;

		for( auto p = creport.data.begin(); p != creport.data.end(); p++ ){
			if ( shouldEntryBeFiltered(p->second)){
				continue;
			}

			countEntries++;

			vector<GroupEntry *> curStack;
			for( GroupEntry * cur = p->first->parent; cur != nullptr; cur = cur->parent) {
				curStack.push_back(cur);					
			}
			for( auto cur = curStack.rbegin(); cur != curStack.rend(); cur++ ){
				messageData << (*cur)->name << '/';
			}
			messageData << p->first->name;
			messageData << '\0';
		}

		// close the group
		messageData << '\0';
	}
	msgData = messageData.str();
}

void MPIReporter::printAggregatedResult(ostream & out, unordered_map<string, EntryGroup> & aggregatedResult){
	out << endl;
	out << "=======================================================================" << endl;
	out << "SIOX Aggregated MPI Report" << endl;
	out << "EntryName = (Average, Min, Max) ProcessCount [ModuleName, ID, Section]" << endl;
	out << "=======================================================================" << endl;

	// sort the component names
	vector< EntryGroup * > sortedGroups;
	sortedGroups.reserve(aggregatedResult.size());
	for ( auto itr = aggregatedResult.begin(); itr != aggregatedResult.end(); itr++ ){
		sortedGroups.push_back(& itr->second);
	}

	sort( sortedGroups.begin(), sortedGroups.end(), [](const EntryGroup * a, const EntryGroup * b) -> bool
		{ 
		    return (a->sectionName < b->sectionName) || ( a->moduleName < b->moduleName ) || ( a->componentID < b->componentID ); 
		});

	// walk through the aggregated results
	for ( auto itr = sortedGroups.begin(); itr != sortedGroups.end(); itr++ ){
		EntryGroup & group = **itr;

		// sort the entries
		vector<const string *> entryNames;
		entryNames.reserve(group.map.size());
		for (auto cur = group.map.begin(); cur != group.map.end(); cur++ ){
			entryNames.push_back( & cur->first );
		}

		sort( entryNames.begin(), entryNames.end(), [](const string * a, const string * b) -> bool
			{
				return *a < *b;
			});

		for (auto cur = entryNames.begin(); cur != entryNames.end(); cur++ ){
			string * name = const_cast<string*>(*cur);
			Entry & entry = group.map[*name];
			out <<  "[" << group.moduleName << ":" << group.componentID << ":\"" << group.sectionName << "\"] " << *name << " = (" << entry.average() << "," << entry.min << "," << entry.max << ") " << entry.processes << endl;
		}
	}
}

bool MPIReporter::shouldEntryBeFiltered(const ReportEntry & entry){
	return entry.priority <= minPriority || ! entry.value.isNumeric();
}

void MPIReporter::processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
	if ( reports.begin() == reports.end() ){
		return;
	}

	int mpiSize = 0;
	int mpiRank = 0;
	PMPI_Comm_size( MPI_COMM_WORLD, & mpiSize );
	PMPI_Comm_rank( MPI_COMM_WORLD, & mpiRank );

	// we expect that components with the same pair of <ID, section> are the same.

	// total number of entries we have locally.
	uint countEntries;
	string msgData;

	packGroupNames( reports, msgData, countEntries );


	uint msgBuffer[2] = { countEntries, (uint) msgData.length() };

	// copy all values into one big array:
	double * values = (double*) malloc( countEntries * sizeof(double) );
	uint valuepos = 0;
	for ( auto itr = reports.begin(); itr != reports.end(); itr++ ){
		const ComponentReport & creport = itr->second;

		for( auto p = creport.data.begin(); p != creport.data.end(); p++ ){
			if ( shouldEntryBeFiltered(p->second)) {
				continue;
			}

			// serialize the value into the string
			values[valuepos++] = p->second.value.toDouble();				
		}
	}
	assert(valuepos == countEntries);

	// contains all the outputs
	unordered_map<string, EntryGroup> aggregatedResult;

	if ( mpiRank == 0 ){
		processMessage( 0, msgBuffer[1], msgBuffer[0], msgData.c_str(), values, aggregatedResult );
		free(values);

		for( int received = 1; received < mpiSize; received++ ){ // we already have our data
			MPI_Status status;
			MPI_Status status2;
			PMPI_Recv( msgBuffer, 2, MPI_UNSIGNED, MPI_ANY_SOURCE, 4711, MPI_COMM_WORLD, & status );

			// cout << status.MPI_SOURCE << ": " << msgBuffer[0] << ":" << msgBuffer[1] << endl;

			// receive the message data.
			char * msgDataRcvd = (char*) malloc( msgBuffer[1] );
			double * valuesRcvd = (double*) malloc( msgBuffer[0] * sizeof(double) );

			PMPI_Recv( msgDataRcvd, msgBuffer[1], MPI_CHAR, status.MPI_SOURCE, 4712, MPI_COMM_WORLD, &status2 );
			PMPI_Recv( valuesRcvd, msgBuffer[0], MPI_DOUBLE, status.MPI_SOURCE, 4713, MPI_COMM_WORLD, &status2 );

			processMessage( status.MPI_SOURCE, msgBuffer[1], msgBuffer[0], msgDataRcvd, valuesRcvd, aggregatedResult );
			
			free(msgDataRcvd);
			free(valuesRcvd);
		}

		printAggregatedResult( out, aggregatedResult );

	}else{
		// cout << mpiRank << ": " << msgBuffer[0] << ":" << msgBuffer[1] << endl;
		// prepare to send all buffers
		PMPI_Send( msgBuffer, 2, MPI_UNSIGNED, 0, 4711, MPI_COMM_WORLD );
		PMPI_Send( const_cast<char*>(msgData.c_str()), msgData.length(), MPI_CHAR, 0, 4712, MPI_COMM_WORLD );
		// send the values
		PMPI_Send( values, countEntries, MPI_DOUBLE, 0, 4713, MPI_COMM_WORLD );

		free(values);
	}
}






extern "C" {
	void * CORE_REPORTER_INSTANCIATOR_NAME()
	{
		return new MPIReporter();
	}
}
