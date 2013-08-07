#ifndef KNOWLEDGE_REASONER_HPP
#define KNOWLEDGE_REASONER_HPP

#include <core/component/Component.hpp>

#include <knowledge/reasoner/ReasoningDatatypes.hpp>
#include <monitoring/datatypes/ids.hpp>

/**
  The reasoner gathers all the information about anomalies and judges the cause/reason of them. 
  It knows the system utilization and tries to resolve 1) process-local, 2) node-global and 3) system-global anomalies.
  If the source of performance degradation is not known, logging will be enabled by the reasoner.
  Each node hosts exactly one reasoner which decides the system status.
  
  Use cases:
	U1 	Anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	U2 	Node-local statistics or anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	U3 	Based on input, reasoner decides to wait 100ms for more anomalies to come, then may/may not trigger log flush.
	U4 	Reasoner communicates current decision (node status) with global reasoner and receives global information, e.g., "File System Occupied"
  
  Requirements:
	R1 	Node-global reasoner communicates with the global reasoner.
	R2	Sink for decisions of process-local anomaly detection plugins.
	R3	Sink for decisions of node-global anomaly detection plugins.
	R4	Triggers logging on node-global Activity- and StatisticsForwarder. 
		Does this only apply to the node-global reasoner, or should the process-local reasoner also filter activities?
 
  Rationales & design decisions/Issues and questions:
	D1 	How do we decide in which system state a node is?
	 	a) We may have a decision table which concludes the limitations based on logical expressions of the observations.
	D2  How are decision tables constructed?
		a) Machine learned
		b) Manual configuration, based on tokens <= This seems more realistic at the moment. 
			b1) Could be stored in a file
			b2) Could be hard-coded in the source-code
			=> Different modules could handle it differently.
	D3	How are link between Multiplexers, plugins and Reasoner created.
		a) Each plugin gets a reference to the "reasoner" and registers itself.
		b) The reasoner gets a list with plugins.
	D4	How is a process-local plugin reporting to the node-global reasoner?
		a) Using a shared-memory communication module. A process-local reasoner implements a facade by 
		which the information is given to the node-global reasoner.
		b) A process-local reasoner is connected to the node-local reasoner. It may reduce/deduce local information before the info is given to the node-global reasoner.
		While this reasoner can not do much it may refine the local information.
	D5 	Do we use the same interface for communicating process-local and node-global information? Are this different interfaces?	
	D6	How are the state-tokens described?
		a) As an enum (more efficient, fixed at compile-time)
		b) As a string (needs more parsing time, better extendable)		
 */

using namespace knowledge::reasoning;

namespace knowledge{

class Reasoner : public core::Component{
public:
	// Report the observation of an activity
	// For which component, which type of operations has the observation been made.
	// What has been observed: (In)efficient operation on another hardware/software component.
	//virtual void reportObservation(activity::Observation o, 
	//	HardwareEntity he, SoftwareEntity se, Reason r, const string & reasonQualifier) = 0;

	// Register the presence of a statistics which can be queried to ask the current utilization.
	// Internally, the relative utilization of such a statistics helps finding the reasons.
	//virtual void registerUtilizationStatistics() = 0;



	// Query statistics about inefficiencies / efficiencies for the whole runtime of the application.
	// Usually done at the end of the application to provide hints about application bottlenecks.
	//virtual void queryUsageStatistics() = 0;
};

}

#define KNOWLEDGE_REASONER_INTERFACE "knowledge_reasoner"

#endif
