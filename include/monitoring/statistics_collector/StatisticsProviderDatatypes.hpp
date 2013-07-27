#ifndef STATISTICS_PROVIDER_DATATYPES_H
#define STATISTICS_PROVIDER_DATATYPES_H

#include <vector>
#include <utility>

#include <monitoring/datatypes/StatisticsTypes.hpp>

using namespace std;

#define LOCAL_HOSTNAME "LOCAL_HOSTNAME"

namespace monitoring{

enum StatisticIntervalType{
	GAUGE, // gauge if interval - We are converting incremental to gauge
	INCREMENTAL,
	SAMPLED
};



class StatisticsProviderDatatypes {
public:
	StatisticsEntity entity;
	StatisticsScope  scope;

	string metrics;

	/* 
	 * The topology defines to which entities the metrics applies.
	 * e.g. (node, device) = ("west1", "0")
	*/

	vector<pair<string,string> > topology;


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
		StatisticsEntity entity, 
		StatisticsScope  scope, 
		string metrics, 
		const vector<pair<string,string> > &  topology,
		StatisticsValue & value,
		enum StatisticIntervalType intervalType,
		string si_unit, 
		string description,
		uint64_t overflow_max_value,
		uint64_t overflow_next_value
		 ) 
		: entity(entity), scope(scope), metrics(metrics), topology(std::move(topology)),value(value), intervalType(intervalType), si_unit(std::move(si_unit)), description(description), overflow_max_value(overflow_max_value), overflow_next_value(overflow_next_value) {}
};

}

#endif