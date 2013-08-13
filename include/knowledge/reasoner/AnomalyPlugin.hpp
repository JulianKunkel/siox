#ifndef KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP
#define KNOWLEDGE_REASONER_ANOMALY_PLUGIN_HPP

#include <set>
#include <memory>

#include <knowledge/reasoner/ReasoningDatatypes.hpp>

using namespace std;

/*
 This interface is triggered by the reasoner, if logging information should be forwarded.
 */
namespace knowledge{

struct AnomalyPluginObservation{
	const ActivityObservation observation;
	const char * issue_cause; // optional: if known. May not be null.

	const UniqueInterfaceID cid;
	const FilesystemID		fid; // optional: if known...
	const ProcessID			pid; // contains the NodeID as well.

	uint32_t occurences;
	int32_t delta_time_ms;
	// maybe we'll add relative delta.

	AnomalyPluginObservation(const ActivityObservation observation, const char * issue_cause, const UniqueInterfaceID cid, const FilesystemID fid, const ProcessID pid, uint32_t occurences, int32_t delta_time_ms) : observation(observation), issue_cause(issue_cause), cid(cid), fid(fid), pid(pid), occurences(occurences), delta_time_ms(delta_time_ms){}


	inline bool operator==(AnomalyPluginObservation const & b) const{
		return observation == b.observation && strcmp(issue_cause, b.issue_cause) && cid == b.cid && fid == b.fid && pid == b.pid;
	}	

	inline bool operator!=(AnomalyPluginObservation const & b) const{
		return ! (*this == b);
	}

	inline bool operator<(AnomalyPluginObservation const & b) const{
		return observation < b.observation ||  strcmp(issue_cause, b.issue_cause) < 0 || cid < b.cid || fid < b.fid || pid < b.pid;
	}

};

// Plugins are registered on three levels.
// Right now all levels are treaded identically, therefore, cid, fid and pid are part of the observation.
// enum class AnomalyPluginLevel : uint8_t{
// 	COMPONENT,
// 	PROCESS,
// 	NODE
// };
//struct AnomalyPluginObservations{
//};


class AnomalyPlugin{
public:
	// After executing this call, the plugin iterates to the next timestep.
	// Ownership of the pointer is transferred to the caller.
	virtual unique_ptr<set<AnomalyPluginObservation>> queryRecentObservations() = 0;
};

}


#endif