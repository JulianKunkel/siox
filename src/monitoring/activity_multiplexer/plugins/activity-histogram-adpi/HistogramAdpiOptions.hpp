//#include <knowledge/optimizer/OptimizerPluginOptions.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

#include <vector>
#include <utility>

using namespace std;
using namespace monitoring;


//@serializable
struct HistogramAdpiOptions : public ActivityMultiplexerPluginOptions{
	// number of buckets for the histogram
	uint16_t buckets = 100;

	// number of buckets that hold abnormal values
	uint16_t extremeBucketCount = 5; 

	// number of buckets for fast/slow values (includes the extremeValues)
	uint16_t slowFastBucketCount = 20; // 20 buckets contain slow values, 20 buckets the fast values

	// after how many operations do we stop the learning count.
	uint16_t learnCount = 1000;
};

