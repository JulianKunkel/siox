#include <mpi.h>

#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream> 

#include <core/reporting/modules/ConsoleReporter/ConsoleReporter.hpp>

#include "MPIReporterOptions.hpp"


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
public:

	void processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		if ( reports.begin() == reports.end() ){
			return;
		}

		char * priorityStr = getenv("SIOX_MPI_REPORTING_PRIORITY");
		uint8_t minPriority = 0;
		if ( priorityStr ){
			minPriority = atoi(priorityStr) ;
		}

		out << endl;
		out << "==========================================================" << endl;
		out << "SIOX Aggregated MPI Report" << endl;
		out << "==========================================================" << endl;

		// we expect that components with the same pair of <ID, section> are the same.

		// total number of entries we have locally.
		uint countEntries = 0;

		// group names serialized
		string msgData;
		{
		// detect sizes and pack messages together
		stringstream messageData;
		for ( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			RegisteredComponent* rc = itr->first;
			messageData << rc->section << '\0' << rc->id << '\0';

			const ComponentReport & creport = itr->second;

			for( auto p = creport.data.begin(); p != creport.data.end(); p++ ){
				if (p->second.priority <= minPriority || ! p->second.value.isNumeric() ){
					continue;
				}

				countEntries++;

				vector<GroupEntry *> curStack;
				for( GroupEntry * cur = p->first->parent; cur != nullptr; cur = cur->parent) {
					curStack.push_back(cur);					
				}
				for( auto cur = curStack.begin(); cur != curStack.end(); cur++ ){
					messageData << (*cur)->name << '\0';
				}
				messageData << '\0';
			}
			

			// close the group
			messageData << '\0';
		}
		msgData = messageData.str();
		}

		int mpiSize = 0;
		int mpiRank = 0;
		PMPI_Comm_size( MPI_COMM_WORLD, & mpiSize );
		PMPI_Comm_rank( MPI_COMM_WORLD, & mpiRank );

		uint msgBuffer[2] = { countEntries, (uint) msgData.length() };

		// copy all values into one big array:
		double values[countEntries];
		uint valuepos = 0;
		for ( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			const ComponentReport & creport = itr->second;

			for( auto p = creport.data.begin(); p != creport.data.end(); p++ ){
				if (p->second.priority <= minPriority || ! p->second.value.isNumeric() ){
					continue;
				}

				// serialize the value into the string
				values[valuepos++] = p->second.value.toDouble();				
			}
		}
		assert(valuepos == countEntries);


		if ( mpiRank == 0 ){
			processMessage( 0, msgBuffer[1], msgBuffer[0], msgData.c_str(), values );

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

				processMessage( status.MPI_SOURCE, msgBuffer[1], msgBuffer[0], msgDataRcvd, valuesRcvd );
				
				free(msgDataRcvd);
				free(valuesRcvd);
			}

		}else{
			// cout << mpiRank << ": " << msgBuffer[0] << ":" << msgBuffer[1] << endl;
			// prepare to send all buffers
			PMPI_Send( msgBuffer, 2, MPI_UNSIGNED, 0, 4711, MPI_COMM_WORLD );
			PMPI_Send( const_cast<char*>(msgData.c_str()), msgData.length(), MPI_CHAR, 0, 4712, MPI_COMM_WORLD );
			// send the values
			PMPI_Send( values, countEntries, MPI_DOUBLE, 0, 4713, MPI_COMM_WORLD );
		}
	}

	void processFinalReport(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		char * outputFilename = getenv("SIOX_MPI_REPORTING_FILENAME");
		processReport(outputFilename, reports);
	}

	virtual ComponentOptions* AvailableOptions(){
		return new MPIReporterOptions();
	}

   virtual void init(){
   }

  	private:

  		/*
  		Process the data received from a single process and aggregate the values to the current set.
  		 */
  		void processMessage( int rank, uint stringLen, uint groupEntries, const char * msgData, const double * values ){  			
  			string componentName;
  			// cout << rank << ":" << stringLen << " " << groupEntries << endl;

  		}
};


extern "C" {
	void * CORE_REPORTER_INSTANCIATOR_NAME()
	{
		return new MPIReporter();
	}
}
