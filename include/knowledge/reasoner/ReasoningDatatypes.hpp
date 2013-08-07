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

// How do we judge the observed behavior?
// For anomaly plugins:
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


// For statistics plugins (note these might cooperate with other activity plugins to come to conclusions):
namespace statistics{
enum Observation{
	LOW,
	AVERAGE,
	HIGH,
	// If we can judge
	UNEXPECTED_LOW_VALUE,
	UNEXPECTED_HIGH_VALUE,
	IRREGULAR_PATTERN
};
}

// For activities, maybe we can specify the demand for each ressource?
enum Ressource{
	CPU,
	MEMORY_BANDWIDTH,
	NETWORK,
	IO,
	CACHE
};

// Where is the cause of an issue?
enum HardwareEntity{
	CLIENT,
	SERVER,
	INFRASTRUCTURE
};

// Causing software-entity
enum SoftwareEntity{
	KERNEL,
	LIBRARY,
	APPLICATION,
	FILESYSTEM	
};

// We may provide the reason if we know already.
enum Reason{
	CPU_UTILIZED,
	MEMORY_UTILIZED,
	NETWORK_UTILIZED,
	IOSUBSYSTEM_UTILIZED,
	KERNEL_USAGE, // Too much time spend in kernel
	BACKGROUND_ACTIVITY, // Much time spend in background activity
	THREAD_COUNT, // Too many threads	
	SUBOPTIMAL_PATTERN, // Suboptimal access pattern, interface usage
	CACHE_HIT, // Data has been cached, therefore the access is faster than usual
	CACHE_MISS // Data has not been cached, therefore the access is slower than usual
};

// May be a good idea to provide an overloaded outputstream for the enums to make them "human" readable.
// TODO: Offer a CPP for this reason.
}

}


#endif