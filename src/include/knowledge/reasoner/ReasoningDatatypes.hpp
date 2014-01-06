#ifndef KNOWLEDGE_REASONER_DATATYPES_HPP
#define KNOWLEDGE_REASONER_DATATYPES_HPP

#include <monitoring/datatypes/ids.hpp>

#include <list>
#include <array>

using namespace monitoring;

namespace knowledge{

//@serializable
struct HealthIssue{
	string name;

	uint32_t occurrences;

	// Estimation in wasted time in ms.
	// Could be negative because we win time due to fast occurrences.
	// How can we derive this value?
	int32_t delta_time_ms;

	bool operator==(const HealthIssue & hi) const{
		return this->name == hi.name && this->occurrences == hi.occurrences && this->delta_time_ms == hi.delta_time_ms;
	}
};

//@serializable
enum HealthState{
	ABNORMAL_FAST = 0,
	FAST = 1,
	OK = 2,
	SLOW = 3,
	ABNORMAL_SLOW = 4,
	ABNORMAL_OTHER = 5,
	HEALTH_STATE_COUNT = 6
};

//@serializable
enum UtilizationIndex{
	CPU = 0,
	MEMORY = 1,
	IO = 2,
	NETWORK = 3,
	UTILIZATION_STATISTIC_COUNT = 4
};

//@serializable
struct Health{
	HealthState overallState;

	array<uint32_t, HEALTH_STATE_COUNT> occurrences; // indexed by HealthState

	list<HealthIssue> positiveIssues;	
	list<HealthIssue> negativeIssues;
};

typedef Health SystemHealth;

typedef Health ProcessHealth;

//@serializable
struct NodeHealth : public Health{
	array<uint8_t, UTILIZATION_STATISTIC_COUNT> utilization; // UtilizationIndex

	NodeHealth() : Health({HealthState::OK, {{0}}, {}, {} }), utilization({{0, 0, 0, 0}}) {}
};


// May be a good idea to provide an overloaded outputstream for the enums to make them "human" readable.
// TODO: Offer a CPP for this reason.
}


#endif
