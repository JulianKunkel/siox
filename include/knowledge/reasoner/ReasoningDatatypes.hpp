#ifndef KNOWLEDGE_REASONER_DATATYPES_HPP
#define KNOWLEDGE_REASONER_DATATYPES_HPP

/*
Anomalies is caused by atypical behavior, e.g. too fast, too slow for the current situation.
They are reported by plugins and assessed by the reasoner.
This file contains enums for Observations, involved Ressources, Entities and Reasons for the cause.

Prosa examples describing typical bottlenecks:
	File system is overloaded because a subset of servers observes an inefficient pattern.
	On a node the network/CPU etc. is utilized too much.
*/

namespace knowledge{
namespace reasoning{

// How do we judge the observed behavior for statistics?
// For statistics plugins (note these might cooperate with other activity plugins to come to conclusions):
// All statistics can be reported, they are identified by the name in the ontology. 
// This means context switches as well as network throughput could be identified.
// So we could theoretically identify the correlations between high/low statistics and observed activities.
namespace statistic{
enum Observation{
	LOW,
	AVERAGE,
	HIGH,
	// If we can judge it to be an anomaly
	UNEXPECTED_LOW_VALUE,
	UNEXPECTED_HIGH_VALUE,
	IRREGULAR_PATTERN
};
}


// How do we judge the observed behavior for anomalies?
namespace activity{
enum Observation{
	// regular behavior
	FAST,
	AVERAGE,
	SLOW,
	// These are anomalies, if we can judge them
	UNEXPECTED_FAST, 
	UNEXPECTED_SLOW, 
	UNEXPECTED_BETWEEN
};
}



// For activities, maybe we can specify the demand for each ressource?
// Maybe we shall try learning the demand of activities automatically?
/*
enum Ressource{
	CPU,
	MEMORY_BANDWIDTH,
	NETWORK,
	IO,
	CACHE
}; 
*/ 

// Where is the cause of slow/fast performance?
enum HardwareEntity{
	CLIENT,
	SERVER,
	INFRASTRUCTURE
};

// Causing software-entity
enum SoftwareEntity{
	LIBRARY,
	APPLICATION,
	KERNEL,
	FILESYSTEM
};

// We may provide the reason if we know already.
enum Reason{
	LOW_RESSOURCE_UTILIZATION, // We will specify which statistics using the ontology ID or domain/name.
	HIGH_RESSOURCE_UTILIZATION,
	// The folloging values will be made available by statistics plugins and therefore are already covered by the LOW/HIGH Utilization.
	//KERNEL_USAGE, // Too much time spend in kernel
	//BACKGROUND_ACTIVITY, // Much time spend in background activity
	//THREAD_COUNT, // Too many threads	

	//For a software-layer cache, the problem arises that we are not capturing these in statistics.
	//Also an activity plugin may indicate that it must be a cache hit/miss.
	CACHE_HIT, // Data has been cached, therefore the access is faster than usual
	CACHE_MISS, // Data has not been cached, therefore the access is slower than usual

	LOAD_IMBALANCE,
	SUBOPTIMAL_PATTERN, // Suboptimal access pattern and/or interface usage. Example, small access-granularity, random-I/O
	UNKNOWN	// We really have to investigate further
};

// May be a good idea to provide an overloaded outputstream for the enums to make them "human" readable.
// TODO: Offer a CPP for this reason.
}

}


#endif