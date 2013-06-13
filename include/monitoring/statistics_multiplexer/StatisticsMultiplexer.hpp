#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <string>

namespace monitoring{

class StatisticsMultiplexerListener : MultiplexerListener<Statistics>{
public:
	string [] requiredMetrics();
};

}

//requiredMetrics(){
// return ["ALL"] is possible !!!
//	return ["metric1", "metric2", NULL];
//}
