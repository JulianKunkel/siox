#ifndef STATISTICSMULTIPLEXERLISTENER_H
#define STATISTICSMULTIPLEXERLISTENER_H

#include <monitoring/datatypes/StatisticsTypes.hpp>
#include <string>

namespace monitoring {


	class StatisticsMultiplexerListener {
		public:
			//virtual string [] requiredMetrics() =0;  // TODO return std::list of ontologies instead of string names of attributes
			//virtual void Notify(Statistics statistics, Attribute & attribute /* TODO:, interval_enum */) =0;
	};


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
