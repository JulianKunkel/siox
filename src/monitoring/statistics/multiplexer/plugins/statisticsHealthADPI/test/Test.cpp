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
	StatisticsMultiplexer * multiplexer = core::module_create_instance<StatisticsMultiplexer>( "", "siox-monitoring-StatisticsMultiplexerSync", STATISTICS_MULTIPLEXER_INTERFACE );

	// wir registrieren das Plugin (normal geschieht dies automatisch)
	StatisticsMultiplexerPlugin * adpi = core::module_create_instance<StatisticsMultiplexerPlugin>( "", "siox-monitoring-statisticsMultiplexerPlugin-healthADPI", STATISTICS_MULTIPLEXER_PLUGIN_INTERFACE );

	// init plugin
	// not necessary, but for testing...
	StatisticsHealthADPIOptions & options = dynamic_cast<StatisticsHealthADPIOptions &>(adpi->getOptions());
	options.multiplexer.componentPointer = multiplexer;

	multiplexer->init();
	adpi->init();

	vector<shared_ptr<Statistic> > statistics;

	StatisticsValue value(0);
	auto statistic = shared_ptr<Statistic>(new Statistic(value, 4711, 3));

	statistics.push_back( statistic );

	multiplexer->notifyAvailableStatisticsChange( statistics, true, true );

	value = 2;
	statistic->update( siox_gettime() );
	multiplexer->newDataAvailable();

	value = 4;
	statistic->update( siox_gettime() );
	multiplexer->newDataAvailable();

	delete( adpi );

	delete( multiplexer );
	cout << "OK" << endl;
}


