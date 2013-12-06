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
		string msgData;
		{
		// detect sizes and pack messages together
		stringstream messageData;
		for ( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			RegisteredComponent* rc = itr->first;
			messageData << rc->section << "|" << rc->id << "|";

			const ComponentReport & creport = itr->second;

			for( auto p = creport.data.begin(); p != creport.data.end(); p++ ){
				if (p->second.priority <= minPriority ){
					continue;
				}

				vector<GroupEntry *> curStack;
				for( GroupEntry * cur = p->first->parent; cur != nullptr; cur = cur->parent) {
					curStack.push_back(cur);					
				}
				for( auto cur = curStack.begin(); cur != curStack.end(); cur++ ){
					messageData << (*cur)->name << "|";
				}
				messageData << "|";
				// serialize the value into the string
				messageData << p->second.value;
			}
			// close the group
			messageData << "|";
		}
		msgData = messageData.str();
		}

		int mpiSize = 0;
		int mpiRank = 0;
		MPI_Comm_size( MPI_COMM_WORLD, & mpiSize );
		MPI_Comm_rank( MPI_COMM_WORLD, & mpiRank );

		uint counts[mpiSize];
		uint mycount = msgData.length();

		MPI_Gather( &mycount, 1, MPI_UNSIGNED, counts, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD );

		if (mpiRank == 0){
			// prepare to receive all buffers
		}else{
			// prepare to send all buffers
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
};


extern "C" {
	void * CORE_REPORTER_INSTANCIATOR_NAME()
	{
		return new MPIReporter();
	}
}
