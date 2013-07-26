#ifndef STATISTICSMULTIPLEXERLISTENER_H
#define STATISTICSMULTIPLEXERLISTENER_H 

#include <monitoring/multiplexer/MultiplexerListener.hpp>
#include <string>

namespace monitoring{

typedef MultiplexerListener<Statistics> StatisticsMultiplexerListener;

class StatisticsMultiplexerListener : MultiplexerListener<Statistics> {
public:
	virtual string [] requiredMetrics() =0;  // TODO return std::list of ontologies instead of string names of attributes
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


}

#endif /* STATISTICSMULTIPLEXERLISTENER_H */
