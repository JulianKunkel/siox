#include <iostream>


#include <monitoring/datatypes/Statistics.hpp>
#include <monitoring/statistics_multiplexer/StatisticsMultiplexer.hpp>
#include <core/module/ModuleLoader.hpp>

using namespace std;
using namespace monitoring;


// set up some StatisticsProvider
// set up some StatisticsCollector

// set up a smux listener

int main( int argc, char * argv[] )
{

	StatisticsMultiplexer * smux1 = core::module_create_instance<StatisticsMultiplexer>( "", "StatisticsMultiplexerSync", "monitoring_statisticsmultiplexer" );


	// maybe pass statistics to smux1


	// watch output in some smux_listener (e.g. like SDBPump would do)


	return 0;
}
