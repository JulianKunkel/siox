#ifndef KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP
#define KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP

#include <memory>
#include <unordered_map>

#include <knowledge/reasoner/ReasoningDatatypes.hpp>

using namespace std;

/*
 This interface is triggered by the reasoner, if logging information should be forwarded.
 */
namespace knowledge {

// Plugins are registered on three levels.
// Right now all levels are treaded identically, theoretically, cid, fid and pid could be part of the observation.
// enum class AnomalyPluginLevel : uint8_t{
//  COMPONENT,
//  PROCESS,
//  NODE
// };


/**
 * Aggregate of a component's health report, as compiled by an ADPI.
 */
struct AnomalyPluginHealthStatistic{
	ComponentID cid;

	array<uint32_t, HEALTH_STATE_COUNT> occurrences;

	unordered_map<string, HealthIssue> positiveIssues;
	unordered_map<string, HealthIssue> negativeIssues;
};


/**
 * Maps components' CIDs to an ADPI-generated health status report for each.
 */
struct HealthStatistics{
	// @todo TODO: Any reason not to use the full CID as a key but only ComponentID.id?
	unordered_map<ComponentID, AnomalyPluginHealthStatistic> map;
};



class AnomalyPlugin {
	public:
		// After executing this call, the plugin iterates to the next timestep.
		// Ownership of the pointer is transferred to the caller.
		virtual unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> queryRecentObservations()
		{
			unordered_map<ComponentID, AnomalyPluginHealthStatistic> * tmp = recentObservations;
			recentObservations = new unordered_map<ComponentID, AnomalyPluginHealthStatistic>();
			return unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>(tmp);
		}

		AnomalyPlugin() {
			recentObservations = new unordered_map<ComponentID, AnomalyPluginHealthStatistic>();
		}

	private:
		unordered_map<ComponentID, AnomalyPluginHealthStatistic> * recentObservations;

	protected:

		void addObservation( ComponentID cid, HealthState state,  const string & issue, int32_t delta_time_ms ) {
			unordered_map<ComponentID, AnomalyPluginHealthStatistic>::iterator find = recentObservations->find( cid );

			if( find == recentObservations->end() ) {
				// append an empty health state
				(*recentObservations)[cid] = { cid };
				find = recentObservations->find( cid );
			}

			AnomalyPluginHealthStatistic & stat = find->second;
			stat.occurrences[state]++;
			if ( issue != "" ){
				if ( state < HealthState::OK ){
					addIssue( stat.positiveIssues, issue, delta_time_ms );
				}
				if ( state > HealthState::OK ){
					addIssue( stat.negativeIssues, issue, delta_time_ms );
				}
			}
		}

		void addIssue( unordered_map<string, HealthIssue> & map, const string & issue, int32_t delta_time_ms ){
			auto find = map.find( issue );
			if ( find == map.end() ){
				map[issue] = { issue, 1, delta_time_ms };
			}else{
				HealthIssue & health = find->second;
				health.occurrences++;
				health.delta_time_ms += delta_time_ms;
			}
		}
};

}


#endif