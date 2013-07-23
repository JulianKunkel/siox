#ifndef STATISTICSFORWARDER_H
#define STATISTICSFORWARDER_H 

#include <monitoring/datatypes/Statistics.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexerListener.hpp>

class StatisticsForwarder : StatisticsMultiplexerListener
{
public:
	void Notify(Statistics * element) =0;

private:
	/* data */
};

#endif /* STATISTICSFORWARDER_H */
