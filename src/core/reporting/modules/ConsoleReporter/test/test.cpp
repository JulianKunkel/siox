#include <core/module/ModuleLoader.hpp>

#include <core/reporting/RuntimeReporter.hpp>

using namespace std;

using namespace core;

class CRI_TestClass : public ComponentReportInterface{
	public:
		shared_ptr<ComponentReport> prepareReport(){
			ComponentReport * rep = new ComponentReport("TestClassInterface", "Impl1");
			rep->data["Stat1"] = {ReportEntry::Type::SIOX_INTERNAL_P1, 4};
			rep->data["Stat2"] = {ReportEntry::Type::APPLICATION, 8};

			return shared_ptr<ComponentReport>(rep);
		}
};

int main( int argc, char const * argv[] )
{
	RuntimeReporter * m1 = core::module_create_instance<RuntimeReporter>( "", "siox-core-reporting-ConsoleReporter", CORE_REPORTER_INTERFACE );

	m1->init();

	CRI_TestClass c1; 

	std::list<std::shared_ptr<ComponentReport>> reports;
	reports.push_back( c1.prepareReport() ) ;

	m1->processFinalReport(reports);

	delete( m1 );
	cout << "OK" << endl;
}


