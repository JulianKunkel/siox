#include <core/module/ModuleLoader.hpp>

#include <core/reporting/RuntimeReporter.hpp>

using namespace std;

using namespace core;

class CRI_TestClass : public ComponentReportInterface, public Component{
	public:
		ComponentReport prepareReport(){
			ComponentReport rep;
			rep.addEntry("Stat1", {ReportEntry::Type::SIOX_INTERNAL_DEBUG, 4});
			rep.addEntry("Stat2", {ReportEntry::Type::APPLICATION_PERFORMANCE, 8});

			return rep;
		}

		void init(){}

		ComponentOptions* AvailableOptions(){ 
			return nullptr; 
		}
};

int main( int argc, char const * argv[] )
{
	RuntimeReporter * m1 = core::module_create_instance<RuntimeReporter>( "", "siox-core-reporting-ConsoleReporter", CORE_REPORTER_INTERFACE );

	m1->init();

	CRI_TestClass c1; 

	std::list< pair<RegisteredComponent*, ComponentReport> > reports;
	RegisteredComponent rc(1, "test", "t1", & c1);
	reports.push_back( { & rc, c1.prepareReport() } ) ;

	m1->processFinalReport(reports);

	delete( m1 );
	cout << "OK" << endl;
}


