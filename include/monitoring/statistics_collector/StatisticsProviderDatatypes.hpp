
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

	const string topology[];

	const string topology_instance[];

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

	/**
	Minimal Measurement interval in ms - Delta sample value / Delta time is good for the moment.
	*/
	uint32_t min_poll_interval_ms;

	// These counters are used for INCREMENTAL intervalType only.
	StatisticsValue overflow_max_value;  // maximum value of the counter
	StatisticsValue overflow_next_value; // if the value overflows we begin with this value.

	StatisticsProviderDatatypes(StatisticsEntity entity, StatisticsScope  scope, ..., StatisticsValue & value) : entity(entity), scope(scope), value(value){}
};
