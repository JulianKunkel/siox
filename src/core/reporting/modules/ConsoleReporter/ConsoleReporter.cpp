#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <vector>

#include <core/reporting/RuntimeReporterImplementation.hpp>
#include "ConsoleReporterOptions.hpp"



using namespace core;

class ConsoleReporter : public RuntimeReporter{
public:

	void processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports){
		if ( reports.begin() == reports.end() ){
			return;
		}

		char * priorityStr 		 = getenv("SIOX_REPORTING_PRIORITY");
		uint8_t minPriority = 0;
		if ( priorityStr ){
			minPriority = atoi(priorityStr) ;
		}

		out << endl;
		out << "==========================================================" << endl;
		out << "SIOX Reports" << endl;
		out << "==========================================================" << endl;

		for( auto itr = reports.begin(); itr != reports.end(); itr++ ){
			const pair<RegisteredComponent*, ComponentReport> & cur  = *itr;
			const RegisteredComponent & rc = * cur.first;

			const ComponentReport & r = cur.second;

			// sort the report 
			vector< GroupEntry * > sortedStr;
			sortedStr.reserve(r.data.size());
			for( auto keyVal = r.data.begin() ; keyVal != r.data.end(); keyVal++ ){
				if( keyVal->second.priority > minPriority ){
					sortedStr.push_back( keyVal->first );
				}
			}
			sort( sortedStr.begin(), sortedStr.end(), sortGroups);


			vector<GroupEntry *> parGroupStack;
			for ( auto key = sortedStr.begin(); key != sortedStr.end() ; key++ ){
				auto elem = r.data.find(*key);
				int id = rc.id > 1000000 ? 0 : rc.id ;

				// walk through the parent group and try to merge as many parents as possible.
				// the stack of the current group
				vector<GroupEntry *> curStack;
				int outputPos = -1;
				uint stackPos = 0;

				for( GroupEntry * cur = (*key)->parent; cur != nullptr; cur = cur->parent, stackPos++ ) {
					if ( outputPos == -1 && (parGroupStack.size() <= stackPos || parGroupStack[stackPos] != cur ) ){
						outputPos = stackPos;
					}
					curStack.push_back(cur);
				}
				// output the missing parents
				for( uint curPos = outputPos; curPos < curStack.size(); curPos++ ){
					cout << curStack[curPos]->name << endl; 
				}

				parGroupStack = curStack;

				out << (*key)->name << " = " << elem->second.value << " [" << rc.moduleName << ":" << id << ":\"" << rc.section << "\"]" << endl;
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

  	private:
  		static bool sortGroups(const GroupEntry * a, const GroupEntry * b){
  					// empty groups come first
  					// sort by group hierarchy
  					// sort by name
    				return ( a->parent == b->parent && a->name < b->name) || ( a->parent == nullptr && b->parent != nullptr ) || ( a->parent != nullptr && b->parent != nullptr && sortGroups(a->parent, b->parent) ) ;
				}
};


extern "C" {
	void * CORE_REPORTER_INSTANCIATOR_NAME()
	{
		return new ConsoleReporter();
	}
}
