#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <string>

namespace monitoring{

class StatisticsMultiplexerListener : MultiplexerListener<Statistics>{
public:
	string [] requiredMetrics();  // TODO return std::list of ontolagies instead of string names of attributes
	void Notify(Statistics statistics, Attribute & attribute);
};

}


//requiredMetrics(){
// return ["ALL"] is possible !!!
//	return ["metric1", "metric2", NULL];
//}
