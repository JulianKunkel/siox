#include <stdlib.h>
#include <fstream>

#include <core/reporting/RuntimeReporterImplementation.hpp>

using namespace core;

class ConsoleReporter : public RuntimeReporter{
public:

	void processReports(ostream & out, const std::list<std::shared_ptr<ComponentReport>> & reports){
		char * priority 		 = getenv("SIOX_REPORTING_PRIORITY");

		for( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			ComponentReport & r = **itr;
			for ( auto keyVal = r.data.begin(); keyVal != r.data.end() ; keyVal++ ){
				out << r.componentType << ":" << r.componentIdentification << " " << keyVal->first << " = " << keyVal->second.value <<  endl;
			}
		}

	}

	void processFinalReport(const std::list<std::shared_ptr<ComponentReport>> & reports){
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
		return new ComponentOptions();
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
