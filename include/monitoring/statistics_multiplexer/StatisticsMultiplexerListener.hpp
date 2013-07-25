#ifndef STATISTICSMULTIPLEXERLISTENER_H
#define STATISTICSMULTIPLEXERLISTENER_H

#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <string>

namespace monitoring{

typedef MultiplexerListener<Statistics> StatisticsMultiplexerListener;

class StatisticsMultiplexerListener : MultiplexerListener<Statistics> {
public:
	virtual string [] requiredMetrics() =0;  // TODO return std::list of ontolagies instead of string names of attributes
	virtual void Notify(Statistics statistics, Attribute & attribute) =0;
};

}

/*
class StatisticsMultiplexerListenerSync : public StatisticsMultiplexerListener
{
};

class StatisticsMultiplexerListenerAsync : public StatisticsMultiplexerListener
{
};
*/


//requiredMetrics(){
// return ["ALL"] is possible !!!
//	return ["metric1", "metric2", NULL];
//}



}

#endif /* STATISTICSMULTIPLEXERLISTENER_H */
