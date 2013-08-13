#ifndef KNOWLEDGE_REASONER_HPP
#define KNOWLEDGE_REASONER_HPP

#include <list>
#include <memory>

#include <core/component/Component.hpp>

#include <monitoring/datatypes/ids.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>
#include <knowledge/reasoner/AnomalyTrigger.hpp>
#include <knowledge/reasoner/QualitativeUtilization.hpp>
#include <knowledge/reasoner/ReasoningDatatypes.hpp>


using namespace std;

namespace knowledge{

/**
  The reasoner gathers all the information about anomalies and judges the cause/reason of them. 
  It knows the system utilization and tries to resolve 1) process-local, 2) node-global and 3) system-global anomalies.
  If the source of performance degradation is not known, logging will be enabled by the reasoner.
  Each node hosts exactly one reasoner which decides the system status.

  There are two sources of anomalies: activities and statistics. Latter is more difficult to judge without knowing activities going on.
  
  Use cases
  ---------
	- U1 	Anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	- U2 	Node-local statistics or anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	- U3 	Based on input, reasoner decides to wait 100ms for more anomalies to come, then may/may not trigger log flush.
	- U4 	Reasoner communicates current decision (node status) with global reasoner and receives global information, e.g., "File System Occupied"
	- U5	At termination of the application, a list of reasons for performance degration is output to the user.
  
  Requirements
  ------------
	- R1 	Node-global reasoner communicates with the global reasoner.
	- R2	Sink for decisions of process-local anomaly detection plugins.
	- R3	Sink for decisions of node-global anomaly detection plugins.
	- R4	Triggers logging on node-global Activity- and StatisticsForwarder. 
		Does this only apply to the node-global reasoner, or should the process-local reasoner also filter activities?
	- R5	Allow access to recent performance issues. These are exchanged with other reasoners.
 
  Rationales & design decisions/Issues and questions
  --------------------------------------------------
	- D1 	How do we decide in which system state a node is?
	 	a) We may have a decision table which concludes the limitations based on logical expressions of the observations.
	- D2  How are decision tables constructed?
		a) Machine learned
		b) Manual configuration, based on tokens <= This seems more realistic at the moment. 
			b1) Could be stored in a file
			b2) Could be hard-coded in the source-code
			=> Different modules could handle it differently.
	- D3	How are link between Multiplexers, plugins and Reasoner created.
		a) Each plugin gets a reference to the "reasoner" and registers itself.
		b) The reasoner gets a list with plugins.
	- D4	How is a process-local plugin reporting to the node-global reasoner?
		a) Using a shared-memory communication module. A process-local reasoner implements a facade by 
		which the information is given to the node-global reasoner.
		b) A process-local reasoner is connected to the node-local reasoner. It may reduce/deduce local information before the info is given to the node-global reasoner.
		While this reasoner can not do much it may refine the local information.
	- D5 	Do we use the same interface for communicating process-local and node-global information? Are this different interfaces?	
	- D6	How are the state-tokens described?
		a) As an enum (more efficient, fixed at compile-time, though)
		b) As a string (needs more parsing time, better extendable)
	- D7	Could we allow compile-time resolving of ontology/values and/or hostnames/filenames?
		This would reduce the runtime overhead and provide a safe correctness-checking.
		Would be useful for many forwarder plugins and reduces the dependency to a facade.
		Drawback: Some components such as the node-local reasoner would know all relevant ontology-IDs at compile-time which is hard to achieve since we hope for extensions.
	- D8	What are recent performance issues and observations?
		As long as an anomaly is reported we
		a) Problem: long-running activities are only flagged as anomaly after they terminated.

	UML Stuff
	---------
	@startuml{reasoner-components.png}
	title Interactions of the Reasoner

	'left to right direction

	frame "Node" {

	frame "Process" {
	    interface Ontology #Yellow
	    interface "System Information" #Gold

	    frame "ComponentX" as Component {

	        frame "AMux Plug-Ins" {
	            component "AnomalyPlugin1" #Orchid
	            component "AnomalyPlugin2" #Orchid
	        }	
	    }

	    interface "AnomalyPlugin" #Orchid

	    component "<b><font size=20>Reasoner" as PReasoner #HotPink
	    AnomalyPlugin1 <- PReasoner : query
	    AnomalyPlugin2 <- PReasoner : query
	    AnomalyPlugin <- PReasoner : query

	    component [siox-ll] #LightGrey

		folder UserOutput
	    note bottom of UserOutput
		The system information and ontology
		are only needed to create human-
		readable user output.
	    end note    
	    [siox-ll] -> UserOutput : create

	    PReasoner <- [siox-ll]: Query usage upon termination


	    PReasoner -.-> [System Information]
	    PReasoner -.-> Ontology

	}


	frame "SIOX Daemon" {
	    component "<b><font size=20>Reasoner" as NReasoner #HotPink

	    PReasoner -> NReasoner : Process state
	    PReasoner <- NReasoner : Node/global state

	    frame "SPlugins" {
		    component QualitativeUtilization #White
	    }

	    component [StatisticsCollector] #Orange    
	    [StatisticsCollector] -> QualitativeUtilization : poll

	    component [SMux] #Orange
	    [StatisticsCollector] -> SMux : Notify

	    frame "SMuxPlugins" {
		    component SForwarder #Orange
	    }
	    SMux -> SForwarder

	    component AForwarder #Orange

	    interface "AnomalyPlugin" as NAnomalyPlugin #Orchid
	    NReasoner -> NAnomalyPlugin : query

		NReasoner -> AForwarder : Trigger
		NReasoner -> SForwarder : Trigger
		NReasoner -> QualitativeUtilization : Query
	}

	}

	component "<b><font size=20>Reasoner" as GReasoner #HotPink
	GReasoner -> NReasoner : Global state
	NReasoner -> GReasoner : Node state


	database "SIOX Transaction System\n" {
		component Activities #Orange
		component Statistics #Orange
	}

	AForwarder -> Activities : History / [Triggered]
	SForwarder -> Statistics : History / [Triggered]

	database "SIOX Knowledge Base\n" {
		component Systeminformation #Orchid
	}

	QualitativeUtilization -> Systeminformation : Query/update stats
	@enduml

################################################################

	@startuml{reasoner-anomaly-plugin-interaction.png}
	AnomalyPlugin1 -> "AP1 set<Observation>" : Observation
	AnomalyPlugin2 -> "AP2 set<Observation>" : Observation
	AnomalyPlugin1 -> "AP1 set<Observation>" : Observation

	activate ReasonerThread
	ReasonerThread -> AnomalyPlugin1 : queryRecentObservations 
	ReasonerThread -> AnomalyPlugin2 : queryRecentObservations
	ReasonerThread -> Reasoner : Update recent PerformanceIssues

	ReasonerThread -> ReasonerThread : Sleep (No anomaly)
	deactivate ReasonerThread

	AnomalyPlugin2 -> "AP2 set<Observation>" : Observation
	AnomalyPlugin2 -> "AP2 set<Observation>" : Observation

	activate ReasonerThread
	ReasonerThread -> AnomalyPlugin1 : queryRecentObservations 
	ReasonerThread -> AnomalyPlugin2 : queryRecentObservations

	ReasonerThread -> AnomalyTrigger1 : Invoke (Anomaly)
	ReasonerThread -> AnomalyTrigger2 : Invoke (Anomaly)
	ReasonerThread -> Reasoner : Update recent PerformanceIssues
	ReasonerThread -> ReasonerThread : Sleep
	deactivate ReasonerThread

	@enduml

################################################################

	@startuml{reasoner-reasoner-interaction.png}

	activate ReasonerThread
	ReasonerThread -> AnomalyPluginX : queryRecentObservations
	ReasonerThread -> Reasoner : Update PerformanceIssues

	ReasonerThread -> Reasoner2 : Push recent PerformanceIssues
	ReasonerThread -> ReasonerThread : Sleep (No anomaly)
	deactivate ReasonerThread

	activate 	Reasoner2Thread
	Reasoner2Thread -> Reasoner : Push recent PerformanceIssues
	Reasoner2Thread -> Reasoner2Thread : Sleep
	deactivate 	Reasoner2Thread


	activate ReasonerThread
	ReasonerThread -> AnomalyPluginX : queryRecentObservations 

	ReasonerThread -> AnomalyTrigger : Invoke (Anomaly)
	ReasonerThread -> Reasoner : Update PerformanceIssues
	ReasonerThread -> Reasoner2 : Push recent PerformanceIssues
	ReasonerThread -> ReasonerThread : Sleep
	deactivate ReasonerThread

	@enduml
*/
class Reasoner : public core::Component{
public:
	// Report the observation of an activity
	// For which component, which type of operations has the observation been made.
	// What has been observed: (In)efficient operation on another hardware/software component.
	// virtual void reportObservation(ActivityObservation o, const IssueLocation & issueLocation, int32_t	delta_time_ms) = 0;

	// // If we know the reason or the location
	// virtual void reportObservation(ActivityObservation o, 
	// 	const IssueLocation & issueLocation,
	// 	const IssueCause & claimedCause,
	// 	const IssueLocation & causeLocation, int32_t delta_time_ms) = 0;

	// // If a statistics anomaly plugin intents to report an anomaly by itself:
	// virtual void reportObservation(StatisticObservation o, OntologyAttributeID statistics_aid, const IssueLocation & issueLocation) = 0;

	// Query a list of current performance causing reasons.
	virtual unique_ptr<list<PerformanceIssue>> queryRecentPerformanceIssues() = 0;

	// Query statistics about inefficiencies / efficiencies for the whole runtime of the application.
	// Usually done at the end of the application to provide hints about application bottlenecks.

	/*
	 Based on the hierachy-level the reasoner keeps different stats for their run-time:
	 A process reasoner has statistics for the process life-time.
	 A daemon maintains stats for its life-time (until daemon is stopped).
	 */
	virtual unique_ptr<list<PerformanceIssue>> queryRuntimePerformanceIssues() = 0;

	// Register the presence of a statistics which can be queried to ask the current utilization.
	// Internally, the relative utilization of such a statistics helps finding the reasons.
	// There can be only one such plugin.
	virtual void connectUtilization(QualitativeUtilization * plugin) = 0;

	// Register an anomaly trigger, all anomaly triggers are notified when an anomaly occurs.
	virtual void connectTrigger(AnomalyTrigger * trigger) = 0;

	virtual void connectAnomalyPlugin(AnomalyPlugin * plugin) = 0;

	// A reasoner is connected either upstream / more global reasoner or downstream.
	// Connection to a remote reasoner is realized using RPC (behind a reasoner facade).
	// Either way the reasoner will forward only refined information
	virtual void connectReasoner(Reasoner * reasoner) = 0;

	// A remote reasoner will report its issues using this call.
	virtual void reportRecentIssues(const Reasoner * reasoner, const set<PerformanceIssue> & issues) = 0;
};

}

#define KNOWLEDGE_REASONER_INTERFACE "knowledge_reasoner"

#endif