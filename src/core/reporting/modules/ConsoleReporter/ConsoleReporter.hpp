#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <vector>

#include <core/reporting/RuntimeReporterImplementation.hpp>
#include <core/reporting/modules/ConsoleReporter/ConsoleReporterOptions.hpp>



using namespace core;

class ConsoleReporter : public RuntimeReporter{
public:
	virtual void processFinalReport(const std::list< pair<RegisteredComponent*, ComponentReport> > & reports);

	virtual ComponentOptions* AvailableOptions();

   virtual void init();

protected:
	void processReport(const char * filename, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports);
	
	virtual void processReports(ostream & out, const std::list< pair<RegisteredComponent*, ComponentReport> > & reports);

	static bool sortGroups(const GroupEntry * a, const GroupEntry * b);
};
