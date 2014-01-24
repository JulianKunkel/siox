#ifndef KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP
#define KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP

#include <memory>
#include <unordered_map>
#include <sstream>
#include <mutex>

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


	friend std::ostream & operator<<( std::ostream & os, const AnomalyPluginHealthStatistic & aphs )
	{
		ostringstream result;

		result << "[CID " << aphs.cid << ": #";
        for( int state = 0; state < HEALTH_STATE_COUNT; state++)
	        result << aphs.occurrences[state] << "#";
	    result << " +" << aphs.positiveIssues.size() << ":";
	    for (auto issue : aphs.positiveIssues)
	    	result << issue.second;
	    result << " -" << aphs.negativeIssues.size() << ":";
	    for (auto issue : aphs.negativeIssues)
	    	result << issue.second;
	    result << "]";

		return os << result.str();
	}
};


/**
 * Maps components' CIDs to an ADPI-generated health status report for each.
 */
struct HealthStatistics{
	// TODO: Any reason not to use the full CID as a key but only ComponentID.id? Any problems resulting?
	unordered_map<ComponentID, AnomalyPluginHealthStatistic> map;


	friend std::ostream & operator<<( std::ostream & os, const HealthStatistics & hs )
	{
		ostringstream result;

		result << "[" << endl;
        for( auto aphs : hs.map )
        	result << "\t" << aphs.second << endl;
		result << "]";

		return os << result.str();
	}
};



class AnomalyPlugin {

	public:

		// After executing this call, the plugin iterates to the next timestep.
		// Ownership of the pointer is transferred to the caller.
		virtual unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>> queryRecentObservations()
		{
			// Disallow other access to aggregated data fields
			unordered_map<ComponentID, AnomalyPluginHealthStatistic> * tmp;
			{
			unique_lock<mutex> dataLock( dataMutex );
			tmp = recentObservations;
			recentObservations = new unordered_map<ComponentID, AnomalyPluginHealthStatistic>();
			}

			return unique_ptr<unordered_map<ComponentID, AnomalyPluginHealthStatistic>>(tmp);
		}


		AnomalyPlugin() {
			recentObservations = new unordered_map<ComponentID, AnomalyPluginHealthStatistic>();
		}

		~AnomalyPlugin(){
			delete( recentObservations );
		}


	private:

		mutex	dataMutex; // To protect access to aggregated data
		unordered_map<ComponentID, AnomalyPluginHealthStatistic> * recentObservations;

		/**
		 * Adds an issue to the local aggregate, assuming mutex protection
		 *
		 * @warning To ensure thread safety, this function requires dataMutex to have been acquired
		 * by the caller!
		 *
		 * @param map [description]
		 * @param issue [description]
		 * @param delta_time_ms [description]
		 */
		void addIssueSafely( unordered_map<string, HealthIssue> & map, const string & issue, int32_t delta_time_ms ){

			auto find = map.find( issue );
			if ( find == map.end() ){
				map[issue] = { issue, 1, delta_time_ms };
			}else{
				HealthIssue & health = find->second;
				health.occurrences++;
				health.delta_time_ms += delta_time_ms;
			}
		}

	protected:

		void addObservation( ComponentID cid, HealthState state,  const string & issue, int32_t delta_time_ms )
		{	// Disallow other access to aggregated data fields
			unique_lock<mutex> dataLock( dataMutex );

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
					addIssueSafely( stat.positiveIssues, issue, delta_time_ms );
				}
				if ( state > HealthState::OK ){
					addIssueSafely( stat.negativeIssues, issue, delta_time_ms );
				}
			}
		}
};

}


#endif