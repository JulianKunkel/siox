#ifndef KNOWLEDGE_REASONER_DATATYPES_HPP
#define KNOWLEDGE_REASONER_DATATYPES_HPP

#include <monitoring/datatypes/ids.hpp>

#include <list>

using namespace std;

/*
Anomalies is caused by atypical behavior, e.g. too fast, too slow for the current situation.
They are reported by plugins and assessed by the reasoner.
This file contains enums for Observations, involved Ressources, Entities and Reasons for the cause.

Prosa examples describing typical bottlenecks:
    File system is overloaded because a subset of servers observes an inefficient pattern.
    On a node the network/CPU etc. is utilized too much.
*/

using namespace monitoring;

namespace knowledge {

// How do we judge the observed behavior for statistics?
// For statistics plugins (note these might cooperate with other activity plugins to come to conclusions):
// All statistics can be reported, they are identified by the name in the ontology.
// This means context switches as well as network throughput could be identified.
// So we could theoretically identify the correlations between high/low statistics and observed activities.
//@serializable	
enum class StatisticObservation: int8_t  {
	    VERY_LOW,
	    LOW,
	    AVERAGE,
	    HIGH,
	    EXTREME_HIGH,
	    // If we can judge it to be an anomaly
	    UNEXPECTED_LOW_VALUE,
	    UNEXPECTED_HIGH_VALUE,
	    IRREGULAR_PATTERN,
	    LEARNED_ENFORCE_ANOMALY
	};


// How do we judge the observed behavior for anomalies?
//@serializable
enum class ActivityObservation : int8_t {
	    // regular behavior
	    VERY_FAST,
	    FAST,
	    AVERAGE,
	    SLOW,
	    VERY_SLOW,
	    // These are anomalies, if we can judge them
	    UNEXPECTED_FAST,
	    UNEXPECTED_SLOW,
	    UNEXPECTED_BETWEEN,
	    LEARNED_ENFORCE_ANOMALY
	};


// For activities, maybe we can specify the demand for each ressource?
// TODO: Maybe we shall try learning the demand of activities automatically?
// Yes, that will be done in an ADPI.



// Where is the slow/fast performance?
// Also: Where is the issue located behind the slow/fast performance?
//@serializable
	class IssueLocation {
		public:
		//@serializable
		enum class Entity : uint8_t {
			    COMPONENT,
			    PROCESS,
			    KERNEL,
			    NODE,
			    // more abstract:
			    FILESYSTEM,
			    CLIENT,
			    SERVER,
			    INFRASTRUCTURE,
			    UNKNOWN
			};

			IssueLocation() {
				memset( this, 0, sizeof( *this ) );
				entity = Entity::UNKNOWN;
			}

			Entity              entity;

			// For local information all these fields are filled, it might become more abstract though.
			ProcessID           pid;
			UniqueInterfaceID   cid; // is invalid, if the issue arrises on node-level.
			FilesystemID        fid; // valid, if the issue arises on a filesystem.

			inline bool operator==( IssueLocation const & b ) const {
				return entity == b.entity && pid == b.pid && cid == b.cid && fid == b.fid;
			}

			inline bool operator!=( IssueLocation const & b ) const {
				return !( *this == b );
			}

			inline bool operator<( IssueLocation const & b ) const {
				return entity < b.entity || pid < b.pid || cid < b.cid || fid < b.fid;
			}

	};


// We may provide the reason if we know already.
//@serializable	
	class IssueCause {
		public:
		//@serializable
		enum class Type : uint8_t {
			    RESSOURCE_UTILIZATION, // We will specify which statistics using the ontology ID or domain/name.
			    // The folloging values will be made available by statistics plugins and therefore are already covered by the LOW/HIGH Utilization.
			    //KERNEL_USAGE, // Too much time spend in kernel //BACKGROUND_ACTIVITY, // Much time spend in background activity   //THREAD_COUNT, // Too many threads

			    //For a software-layer cache, the problem arises that we are not capturing these in statistics.
			    //Also an activity plugin may indicate that it must be a cache hit/miss.
			    CACHING, // Data has/or has not been cached, therefore the access is faster/slower than usual

			    LOAD_IMBALANCE,
			    SUBOPTIMAL_PATTERN, // Suboptimal access pattern and/or interface usage. Example, small access-granularity, random-I/O
			    UNKNOWN // We really have to investigate further
			};

			IssueCause() {
				memset( this, 0, sizeof( *this ) );
				type = Type::UNKNOWN;
			}

			Type type;

			OntologyAttributeID statistics_aid; // statistics which causes the issue. May be 0.
			string str_qualifier; // an arbitrary string defining the cause further

			inline bool operator==( IssueCause const & b ) const {
				return type == b.type && statistics_aid == b.statistics_aid &&
				       str_qualifier == b.str_qualifier;
			}

			inline bool operator!=( IssueCause const & b ) const {
				return !( *this == b );
			}

			inline bool operator<( IssueCause const & b ) const {
				return type < b.type || statistics_aid < b.statistics_aid ||  str_qualifier < b.str_qualifier ;
			}

	};

//@serializable
	class PerformanceIssue {
		public:
		//@serializable
		enum class Quality: uint8_t {
			    GOOD_PERFORMANCE,
			    BAD_PERFORMANCE,
			    NO_ISSUE
			};


			// Quality / Type of the issue.
			Quality       quality;

			// Where is the issue observed. We expect the GOOD or BAD performance becomes apparent on this component.
			IssueLocation issueLocated;

			// Where is the cause located
			IssueLocation causeLocation;

			// What is the cause of the isse.
			IssueCause cause;

			// Number of occurences
			uint32_t    occurences;

			// Estimation in wasted time in ms.
			// May be negative because we win time due to fast operations.
			// How can we derive this value?
			int32_t delta_time_ms;

			PerformanceIssue( Quality quality, IssueLocation issueLocated, IssueLocation causeLocation, IssueCause cause, int32_t delta_time_ms ) : quality( quality ), issueLocated( issueLocated ), causeLocation( causeLocation ), cause( cause ), occurences( 1 ), delta_time_ms( delta_time_ms ) {}

			PerformanceIssue() : quality( Quality::NO_ISSUE ) {}

			inline bool operator==( PerformanceIssue const & b ) const {
				return quality == b.quality && issueLocated == b.issueLocated && causeLocation == b.causeLocation && cause == b.cause;
			}

			inline bool operator!=( PerformanceIssue const & b ) const {
				return !( *this == b );
			}

			inline bool operator<( PerformanceIssue const & b ) const {
				return quality < b.quality || issueLocated < b.issueLocated || causeLocation < b.causeLocation || cause < b.cause;
			}
	};


// May be a good idea to provide an overloaded outputstream for the enums to make them "human" readable.
// TODO: Offer a CPP for this reason.
}


#endif
