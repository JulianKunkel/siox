#include <list>

#include <util/ReporterHelper.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <core/reporting/RuntimeReporter.hpp>


using namespace std;
using namespace core;

namespace util{

void invokeAllReporters(ComponentRegistrar * registrar){
	list< pair<ComponentReportInterface*, RegisteredComponent*> > lst = registrar->listAllComponentsOfATypeFull<ComponentReportInterface>();

	std::list< pair<RegisteredComponent*, ComponentReport> > reports;
	for( auto c = lst.begin(); c != lst.end(); c++ ){		
		reports.push_back( { c->second, c->first->prepareReport() } ) ;
	}

	list< RuntimeReporter*> reporters = registrar->listAllComponentsOfAType<RuntimeReporter>();

	for( auto r = reporters.begin(); r != reporters.end(); r++ ){
		(*r)->processFinalReport( reports );
	}
}

}
