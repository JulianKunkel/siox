#ifndef STATISTICS_PROVIDER_DATATYPES_H
#define STATISTICS_PROVIDER_DATATYPES_H

#include <vector>
#include <utility>

#include <core/math/MathematicalEquation.hpp>
#include <monitoring/statistics/StatisticsTypes.hpp>
#include <monitoring/datatypes/Topology.hpp>

using namespace std;

namespace monitoring {

	enum StatisticIntervalType {
		GAUGE, // gauge if interval - We are converting incremental to gauge
		INCREMENTAL,
		SAMPLED
	};

	const string kStatisticsDomain = "statistics";

	class StatisticsProviderDatatypes {
		public:
			string metrics;

			/*
			 * The topology defines to which entities the metrics applies.
			 * This is a topology path that may start with an alias like "@localhost/sda:1".
			 */
			string topologyPath;


			// This is a pointer to the value which is updated by the SPlugin.
			StatisticsValue & value;

			enum StatisticIntervalType intervalType;

			/**
			The international system unit
			*/
			string si_unit;

			/**
			Details if any, otherwise empty ""
			*/
			string description;

			// These counters are used for INCREMENTAL intervalType only.
			StatisticsValue overflow_max_value;  // maximum value of the counter
			StatisticsValue overflow_next_value; // if the value overflows we begin with this value.

			StatisticsProviderDatatypes(
			    const string& metrics,
			    const string& topologyPath,
			    StatisticsValue & value,
			    enum StatisticIntervalType intervalType,
			    const string& si_unit,
			    const string& description,
			    uint64_t overflow_max_value = 0,
			    uint64_t overflow_next_value = 0
			)
				:
				metrics( metrics ),
				topologyPath( topologyPath ),
				value( value ),
				intervalType( intervalType ),
				si_unit( std::move( si_unit ) ),
				description( description ),
				overflow_max_value( overflow_max_value ),
				overflow_next_value( overflow_next_value )
			{}
	};


	/*
	 * This datatype describes useful metrics which can be computed from the existing metrics.
	 * Every INCREMENTAL and GAUGE counter will be automatically derived to the average value over the interval.
	 * Example:
	 *   INPUT_OUTPUT, NODE, "quantity/block/reads", {{"node", LOCAL_HOSTNAME}, {"device", name}}, INCREMENTAL, "", "Field 1 -- # of reads issued"
	 * Will be automatically derived in the collector to:
	 *   INPUT_OUTPUT, NODE, "quantity/block/reads", {{"node", LOCAL_HOSTNAME}, {"device", name}}, "/s", "Field 1 -- # of reads issued"
	 *
	 * Problem:
	 * Change in the metrics ontology, e.g. quantity/block/dataRead becomes throughput/block/dataRead
	 * Another example:
	 * "time/block/access" becomes "throughput/block/access"
	 *
	 * We could provide derived metrics for device classes, for the whole node, depending on a semantics such as "network/filesystem".
	 * For each counter we could offer average and aggregated value.
	 * => We need mathematical equations that are evaluated.
	 *
	 * How could one specify such equations?
	 * 1) Aggregation of all ethX devices into a single statistic per node.
	 *    e.g. "quantity/network/ethernet/eth*"
	 *
	 * 2) Aggregation of "quantity/blocks/dataRead" and "quantity/blocks/reads" into "quantity/blocks/accessGranularity"
	 *
	 * Most of 1) and 2) is offered by a single plugin, but some such as emantical information which devices belong together to "filesystem" is not known by a StatisticsProvider and is provided by the Collector.
	 * Some derived statics might need statistics from multiple StatisticsProviders. These statitistics must be described in the Collector.
	 *
	 * {INPUT_OUTPUT, NODE, "time/block/access", {{"node", LOCAL_HOSTNAME}, {"device", name}}, cur[9], INCREMENTAL, "ms", "Field 10 -- # of milliseconds spent doing I/Os", overflow_value, 0}
	 *
	 * The collector must calculate the dependencies between the DerivedDatatypes and evaluate them accordingly.
	 */
	class StatisticsDerivedDatatype : public StatisticsProviderDatatypes {
		public:

			/*
			 * The superclass describes the derived metrics we create here.
			 * A Description may use wildcards in the "metrics" field and in the "topology" instances
			 * to match multiple metrics such as eth*.
			 * A variable of such a metric may contain multiple values.
			 * Therefore it is mandatory apply reduction operators (see below).
			 */
			vector<pair<string, StatisticsDescription>> variables;

			/* Equation to compute the new metrics based on existing metrics.
			 * Whenever any metrics changes we have to recompute the value, at best the update interval is identical for all metrics.
			 * Existing reduction operators: average, sum, min, max, count (this is the number of variables)
			 * Example equations, variables are specified above (the return type is only added for illustrating purpose):
			 * averageOPsize = size / count
			 *
			 * nodeThroughput = sum(ethTp)
			 * errorRate = sum(ethTp) / sum(packetDropRate)
			 */
			core::MathematicalEquation equation;
	};

}

#endif
