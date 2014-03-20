#include <core/module/ModuleLoader.hpp>

#include <monitoring/statistics/multiplexer/StatisticsMultiplexer.hpp>
#include <monitoring/statistics/multiplexer/StatisticsMultiplexerPlugin.hpp>

#include <util/time.h>

#include <monitoring/statistics/multiplexer/plugins/statisticsHealthADPI/StatisticsHealthADPIOptions.hpp>

using namespace std;

using namespace monitoring;
using namespace core;

int main( int argc, char const * argv[] )
{
	StatisticsMultiplexer * m1 = core::module_create_instance<StatisticsMultiplexer>( "", "siox-monitoring-StatisticsMultiplexerSync", STATISTICS_MULTIPLEXER_INTERFACE );

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	StatisticsMultiplexerPlugin * ap = core::module_create_instance<StatisticsMultiplexerPlugin>( "", "siox-monitoring-statisticsMultiplexerPlugin-healthADPI", STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE );

	// init plugin
	// not necessary, but for testing...
	StatisticsHealthADPIOptions & op = dynamic_cast<StatisticsHealthADPIOptions &>(ap->getOptions());
	op.multiplexer.componentPointer = m1;

	m1->init();
	ap->init();

	vector<shared_ptr<Statistic> > statistics;

	StatisticsValue value(0);
	auto stat = shared_ptr<Statistic>(new Statistic(value, 4711, 3));

	statistics.push_back( stat );

	m1->notifyAvailableStatisticsChange( statistics, true, true );

	value = 2;
	stat->update( siox_gettime() );
	m1->newDataAvailable();

	value = 4;
	stat->update( siox_gettime() );
	m1->newDataAvailable();

	delete( ap );

	delete( m1 );
	cout << "OK" << endl;
}


