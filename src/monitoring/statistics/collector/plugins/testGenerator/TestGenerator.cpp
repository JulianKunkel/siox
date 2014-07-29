#include <monitoring/statistics/collector/plugins/testGenerator/TestGeneratorOptions.hpp>
#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>


#include <stdint.h>
#include <unistd.h>

using namespace std;
using namespace monitoring;
using namespace core;

class TestGenerator: public StatisticsProviderPlugin {
public:
	ComponentOptions* AvailableOptions() override;
	virtual void nextTimestep() throw() override;
	virtual vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
private:
	vector<StatisticsValue> values;
};

ComponentOptions* TestGenerator::AvailableOptions() {
	return new TestGeneratorOptions();
}

void TestGenerator::nextTimestep() throw() {
	auto options = getOptions<TestGeneratorOptions>();
	for (int i=0; i < options.statisticsToCreate ; i++ ){
		values[i] = options.minValue + (options.maxValue - options.minValue) * (rand() / (double) RAND_MAX);
	}
}

vector<StatisticsProviderDatatypes> TestGenerator::availableMetrics() throw() {
	vector<StatisticsProviderDatatypes> r;
	auto options = getOptions<TestGeneratorOptions>();

	char name[100];

	values.resize(options.statisticsToCreate);
	for (int i=0; i < options.statisticsToCreate ; i++ ){
		sprintf(name, "test/val%d", i);
		values[i] = 0.0;
		r.push_back( {name, "@localhost", values[i], GAUGE, "", "Test values"} );
	}

	return r;
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME()
	{
		return new TestGenerator();
	}
}
