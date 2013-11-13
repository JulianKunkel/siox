#include <stdlib.h>
#include <fstream>

#include <core/reporting/RuntimeReporterImplementation.hpp>
#include "ConsoleReporterOptions.hpp"

using namespace core;

class ConsoleReporter : public RuntimeReporter{
public:

	void processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		char * priority 		 = getenv("SIOX_REPORTING_PRIORITY");

		if ( reports.begin() == reports.end() ){
			return;
		}

		out << endl;
		out << "==========================================================" << endl;
		out << "SIOX Reports" << endl;
		out << "==========================================================" << endl;

		for( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			const pair<RegisteredComponent*, ComponentReport> & cur  = *itr;
			const RegisteredComponent & rc = * cur.first;

			const ComponentReport & r = cur.second;
			for ( auto keyVal = r.data.begin(); keyVal != r.data.end() ; keyVal++ ){
				out << keyVal->first << " = " << keyVal->second.value << " [" << rc.moduleName << ":" << rc.id << ":\"" << rc.section << "\"]" << endl;
				// r.componentType << ":" << 
			}
		}

	}

	void processFinalReport(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		char * outputFilename = getenv("SIOX_REPORTING_FILENAME");

		if( outputFilename != nullptr ){
			ofstream file( outputFilename );
    		if ( file.fail() ) {
    			cerr << "SIOX Reporting could not open file: \"" << outputFilename << "\" (environment variable SIOX_REPORTING_FILENAME)" << endl;
    			return;
    		}

			processReports(file, reports);
			file.close();
		}else{
			processReports( cout, reports );
		}
	}

	virtual ComponentOptions* AvailableOptions(){
		return new ConsoleReporterOptions();
	}

   virtual void init(){

   }
};


extern "C" {
	void * CORE_REPORTER_INSTANCIATOR_NAME()
	{
		return new ConsoleReporter();
	}
}
