#ifndef KNOWLEDGE_REASONER_DATATYPES_HPP
#define KNOWLEDGE_REASONER_DATATYPES_HPP

#include <monitoring/datatypes/ids.hpp>

#include <list>
#include <array>
#include <unordered_map>
#include <sstream>
#include <iostream>

namespace knowledge{


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

//Additional statistics of the node we would like to transfer to processes.
//@serializable
enum NodeStatisticsIndex{
	CPU_SECONDS = 0,
	ENERGY_JOULE = 1,
	MEMORY_BYTES = 2,
	NETWORK_BYTES = 3,
	IO_BYTES = 4,
	NODE_STATISTIC_COUNT = 5
};


//@serializable
struct HealthIssue{
	string name;

	uint32_t occurrences;

	// Estimation in wasted time in ms.
	// Could be negative because we win time due to fast occurrences.
	// How can we derive this value?
	int32_t delta_time_ms;


	HealthIssue()
	{
		this->name = "unspecified issue";
		this->occurrences = 0;
		this->delta_time_ms = 0;
	}

	HealthIssue( string name )
	{
		this->name = name;
		this->occurrences = 0;
		this->delta_time_ms = 0;
	}

	HealthIssue( string name, uint32_t occurrences, int32_t delta_time_ms)
	{
		this->name = name;
		this->occurrences = occurrences;
		this->delta_time_ms = delta_time_ms;
	}


	bool operator==(const HealthIssue & hi) const
	{
		return this->name == hi.name && this->occurrences == hi.occurrences && this->delta_time_ms == hi.delta_time_ms;
	}


	void add( const HealthIssue & hi )
	{
		if ( this->name == hi.name ){
			this->occurrences += hi.occurrences;
			this->delta_time_ms += hi.delta_time_ms;
		}
		else {
			cout << "Error: HealthIssue \"" << hi.name << "\" cannot be aggregated into \"" << this->name << "\"!" << endl;
		}
	}


	friend std::ostream & operator<<( std::ostream & os, const HealthIssue & issue )
	{
		ostringstream result;

		result << "[\"" << issue.name << "\", ";
        result << issue.occurrences << "x, ";
	    result << issue.delta_time_ms << " ms";
	    result << "]";

		return os << result.str();
	}
};


//@serializable
struct HealthIssueList{
	list<HealthIssue> issues;

	HealthIssueList() : issues() {}


	bool operator==( const HealthIssueList & hil ){
		return this->issues == hil.issues;
	}


	void add( const HealthIssue & hi ){
		for( auto itr = this->issues.begin(); itr != this->issues.end(); itr++ ){
			if( itr->name == hi.name ) {
				itr->add(hi);
				return;
			}
		}
		// Not found
		this->issues.push_back(hi);
	}


	void add( const HealthIssueList & hil ){
		for( auto itr = hil.issues.begin(); itr != hil.issues.end(); itr++ ){
			this->add(*itr);
		}
	}


	friend std::ostream & operator<<( std::ostream & os, const HealthIssueList & list )
	{
		ostringstream result;

		result << "[ " << list.issues.size() << "issues:" << endl;
        for( auto issue : list.issues )
        	result << "\t" << issue << endl;
	    result << "]";

		return os << result.str();
	}
};


struct HealthIssueMap{
	unordered_map<string, HealthIssue> issues;

	HealthIssueMap() : issues() {}

	bool operator==( const HealthIssueMap & him ){
		return this->issues == him.issues;
	}

	void add( const HealthIssueList & hil ){
		for( auto itr = hil.issues.begin(); itr != hil.issues.end(); itr++ ){
			auto issue = this->issues.find( itr->name );
			if( issue != this->issues.end() ){
				issue->second.add(*itr);
			}else{
				this->issues[itr->name] = *itr;
			}
		}
	}

	void add( const HealthIssueMap & him ){
		for( auto itr = him.issues.begin(); itr != him.issues.end(); itr++ ){
			auto issue = this->issues.find( itr->first );
			if( issue != this->issues.end() ){
				issue->second.add(itr->second);
			}else{
				this->issues[itr->first] = itr->second;
			}
		}
	}


	friend std::ostream & operator<<( std::ostream & os, const HealthIssueMap & map )
	{
		ostringstream result;

		result << "[ " << map.issues.size() << "issues:" << endl;
        for( auto issue : map.issues )
        	result << "\t" << issue.second << endl;
	    result << "]";

		return os << result.str();
	}
};


//@serializable
struct Health{

	Timestamp timeLastModified; // The last time any modifications were made to the object's data

	HealthState overallState;

	array<uint32_t, HEALTH_STATE_COUNT> occurrences; // indexed by HealthState

	list<HealthIssue> positiveIssues;
	list<HealthIssue> negativeIssues;


	Health(){

		timeLastModified = time(0);
		overallState = HealthState::OK;
		for ( int i = 0; i < HEALTH_STATE_COUNT; ++i )
			occurrences[i] = 0;
	}

	Health( HealthState state ){

		timeLastModified = time(0);
		overallState = state;
		for ( int i = 0; i < HEALTH_STATE_COUNT; ++i )
			occurrences[i] = 0;
	}

	Health( HealthState state, array<uint32_t, HEALTH_STATE_COUNT> nOccurrences, list<HealthIssue> posIssues, list<HealthIssue> negIssues) :
		overallState(state),
		occurrences(nOccurrences),
		positiveIssues(posIssues),
		negativeIssues(negIssues){

		timeLastModified = time(0);
	}

	friend std::ostream & operator<<( std::ostream & os, const Health & h )
	{
		ostringstream result;

		result << "State:    \t" << to_string(h.overallState) << endl;
		result << "Occurrences:\t";
		for (auto o : h.occurrences )
			result << " | " << o;
		result << " | " << endl;
		result << "Issues:\t\t";
		result << "+" << h.positiveIssues.size() << "\t";
		result << "-" << h.negativeIssues.size() << endl;
		result << "Last Modified:\t" << h.timeLastModified << endl;

		return os << result.str();
	}
};

typedef Health SystemHealth;

typedef Health ProcessHealth;

//@serializable
struct NodeHealth : public Health{
	array<uint8_t, UTILIZATION_STATISTIC_COUNT> utilization; // UtilizationIndex
	
	// the following elements account for consumed resources
	array<float, NODE_STATISTIC_COUNT> statistics;

	NodeHealth(){
		for ( int i = 0; i < UTILIZATION_STATISTIC_COUNT; ++i ){
			utilization[i] = 0;
		}
		for ( int i = 0; i < NODE_STATISTIC_COUNT; ++i ){
			statistics[i] = 0;
		}
	}

	friend std::ostream & operator<<( std::ostream & os, const NodeHealth & h )
	{
		ostringstream result;

		result << "State:    \t" << to_string(h.overallState) << endl;
		result << "Utilization:\t";
		for ( auto util : h.utilization )
			result << " | " << (int) util;
		result << " | " << endl;
		result << "Occurrences:\t";
		for ( auto o : h.occurrences )
			result << " | " << o;
		result << " | " << endl;
		result << "Issues:\t\t";
		result << "+" << h.positiveIssues.size() << "\t";
		result << "-" << h.negativeIssues.size() << endl;
		result << "Last Modified:\t" << h.timeLastModified << endl;

		return os << result.str();
	}
/*
*/
};


/*
	struct HealthIssueWithExplaination : HealthIssue{
		// The node and global issues, that might explain the performance issues:
		unordered_map<string, HealthIssue> globalIssues;
		unordered_map<string, HealthIssue> nodeLocalIssues;
	};
*/

	/*
	 Remember and add the local issues forever.
	 */
/*
	struct HealthStatistic{
		UniqueInterfaceID cid; // can be mapped to the name later
		unordered_map<string, HealthIssueWithExplaination> issues;
	};
*/


// May be a good idea to provide an overloaded outputstream for the enums to make them "human" readable.
// TODO: Offer a CPP for this reason.

inline string toString(HealthState s){
	switch(s){
		case ABNORMAL_SLOW:
			return "ABNORMAL_SLOW";
		case ABNORMAL_FAST:
			return "ABNORMAL_FAST";
		case ABNORMAL_OTHER:
			return "ABNORMAL_OTHER";
		case FAST:
			return "FAST";
		case OK:
			return "OK";
		case SLOW:
			return "SLOW";
		default:
			return "UNKNOWN";
	}
}

inline string toString(UtilizationIndex s){
	switch(s){
		case CPU:
			return "CPU";
		case MEMORY:
			return "MEMORY";
		case IO:
			return "IO";
		case NETWORK:
			return "NETWORK";
		default:
			return "UNKNOWN";
	}
}

} // namespace knowledge

#endif
