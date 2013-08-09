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

  There are two sources of anomalies: activities and statistics. Latter is more difficult to judge without knowing activities going on.
  
  Use cases:
	U1 	Anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	U2 	Node-local statistics or anomaly detection plugin reports an anomaly (observation, potential limitations and causes).
	U3 	Based on input, reasoner decides to wait 100ms for more anomalies to come, then may/may not trigger log flush.
	U4 	Reasoner communicates current decision (node status) with global reasoner and receives global information, e.g., "File System Occupied"
	U5	At termination of the application, a list of reasons for performance degration is output to the user.
  
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
		a) As an enum (more efficient, fixed at compile-time, though)
		b) As a string (needs more parsing time, better extendable)
	D7	Could we allow compile-time resolving of ontology/values and/or hostnames/filenames?
		This would reduce the runtime overhead and provide a safe correctness-checking.
		Would be useful for many forwarder plugins and reduces the dependency to a facade.
		Drawback: Some components such as the node-local reasoner would know all relevant ontology-IDs at compile-time which is hard to achieve since we hope for extensions.
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

/* UML Stuff:
@startuml reasoner-interactions.png
title Interactions of the Reasoner

'left to right direction

frame "Node" {

frame "Process" {
    interface Ontology #Yellow
    interface "System Information" #Gold

    frame "ComponentX" as Component {

        frame "AMux Plug-Ins" {
            component "ADPI1" #Orchid
            component "ADPI2" #Orchid
        }	
    }

    component "<b><font size=20>Reasoner" as PReasoner #HotPink
    ADPI1 -> PReasoner
    ADPI2 -> PReasoner

    folder [User output] #LightGrey
    PReasoner -> [User output] : Create on process-termination


    PReasoner -.-> [System Information]
    PReasoner -.-> Ontology

    note bottom of [User output]
	The system information and ontology
	are only needed to create human-
	readable user output.
    end note
}


frame "SIOX Daemon" {
    component "<b><font size=20>Reasoner" as NReasoner #HotPink

    PReasoner -> NReasoner : Process state
    PReasoner <- NReasoner : Node/global state

    frame "SPlugins" {
	    component RelativeUtilization #White
    }

    component [StatisticsCollector] #Orange    
    [StatisticsCollector] -> RelativeUtilization : poll

    component [SMux] #Orange
    [StatisticsCollector] -> SMux : Notify

    frame "SMuxPlugins" {
	    component SForwarder #Orange
    }
    SMux -> SForwarder

    component AForwarder #Orange

	NReasoner -> AForwarder : Trigger
	NReasoner -> SForwarder : Trigger
	NReasoner -> RelativeUtilization : Query
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


RelativeUtilization -> Systeminformation : Query/update stats

@enduml
 */