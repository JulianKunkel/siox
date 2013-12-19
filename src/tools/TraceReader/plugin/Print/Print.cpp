#include <regex>
#include <unordered_map>

#include <monitoring/ontology/Ontology.hpp>
#include <tools/TraceReader/plugin/Print/Print.hpp>


using namespace std;
using namespace monitoring;
using namespace tools;

void PrintPlugin::moduleOptions(program_options::options_description & od){

}
void PrintPlugin::init(program_options::variables_map * vm, TraceReader * tr){

}

Activity * PrintPlugin::processNextActivity(Activity * a){
	tr->printActivity( a );
	return a;
}

extern "C" {
	void * TOOLS_TRACE_READER_INSTANCIATOR_NAME()
	{
		return new PrintPlugin();
	}
}