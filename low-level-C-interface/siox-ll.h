/**
 * @file siox-ll.h SIOX low-level interface headers.
 *
 * @date 11/2012
 * @copyright GNU Public License
 * @authors Michaela Zimmer, Julian Kunkel, Marc Wiedemann & Alvaro Aguilera
 *  
 * @mainpage
 * 
 * @section intro Introduction
 * 
 * Applications running on HPC systems use a file system to do their I/O. This 
 * mostly consists of the initial read of the input data, the periodical storage 
 * of checkpointing information from which to restore the execution state  in case 
 * of unexpected program termination, as well as of the eventual writing of the 
 * application's actual output data.
 * 
 * In order for the I/O operations not to become the scalability bottleneck of 
 * HPC applications, the file system and I/O infrastructure must keep pace with 
 * the increasing performance and number of computing cores present in HPC 
 * systems. In this context, a global optimization of the file system turns out 
 * to be very difficult to impossible. This is in part due to the disparate nature
 * of the requirements and expectations of different user groups, and in part 
 * because currently there is no way to identify abnormal I/O behavior and trace 
 * it back to its source.
 * 
 * SIOX's main goal is to gain an overview of all the I/O activity taking place 
 * on an HPC system, and to use this information to optimize it. Initially, the 
 * project's scope spans the development of standardized interfaces to collect, 
 * reduce, and store performance data from all relevant layers. This information 
 * will then be analyzed and correlated with previously observed access patterns 
 * in order to gain an understanding of the characteristics and causal 
 * relationships of the system.
 *
 * @image html datacollection.png "SIOX in an HPC System"
 * 
 * This knowledge will be the starting point for subsequent performance 
 * optimizations aimed at specific users and applications, carried out through 
 * e.g. the automatic tuning of Open MPI or file system parameters. Such 
 * use-profiles are going to be continuously created and not only helpful for 
 * optimization, but also when diagnosing acute performance problems, or when 
 * planning new acquisitions. In the course of the project, an holistic approach 
 * for I/O analysis should be conceived, implemented and applied. While SIOX' 
 * applicability is oriented towards HPC environments, it shouldn't be 
 * constricted to them. In this way, the integrated analysis of applications, 
 * file systems, and infrastructure could also be used for the future 
 * optimization of other scenarios e.g. the design of file system caches for 
 * mail servers. 
 * 
 * The following sections describe the current state of the project.
 * 
 * @section architecture SIOX Architecture and Workflow
 * 
 * While work on the definite architecture of the SIOX system is still in 
 * progress, some of its essential components are already well defined. Among them
 * are the SIOX daemons, the knowledge base and the central data warehouse. The
 * workflow between the different components is illustrated in the figure below.
 * 
 * @image html architecture.png "SIOX Workflow"
 * 
 * @subsection client SIOX Client
 * 
 * The SIOX client serves as the interface between instrumented software 
 * (libraries and applications) and remote SIOX servers. It is implemented 
 * as a daemon whose job includes the correlation of the local I/O activities, 
 * reduction of redundancy, aggregation of information and the transmission of 
 * relevant performance and trace data to the SIOX servers. 
 * 
 * @subsection servers SIOX Servers
 *
 * The server functionality is accomplished in SIOX by a group of separate 
 * server processes fulfilling different roles. 
 *
 * @subsubsection transaction Transaction System
 * 
 * The transaction system is responsible for the continuous collection and 
 * correlation across system boundaries of the flow of trace information sent by 
 * the distributed clients. The result produced by the transaction system is 
 * the causal chain of I/O events produced by an I/O activity at the application
 * layer, as well as a detailed performance evaluation of the relevant links in
 * order to reveal potential I/O bottlenecks. This information is then sent to 
 * the data warehouse server for storage.
 * 
 * @subsubsection datawarehouse Data Warehouse
 * 
 * The data warehouse server manages a persistent archive of historical I/O
 * information and the corresponding observed performance data. It decides 
 * whether the informaiton is worth keeping or not, and if so, it stores it in a
 * database for future consultation. The data warehouse is used periodically by 
 * the knowledge base and the transactions server.
 * 
 * @subsubsection knowledgebase Knowledge Base
 * 
 * The goal of the knowledge base is to capitalize on the historical data 
 * accumulated in the data warehouse to not only hint the user about possible 
 * I/O bottlenecks in his application, but also to autonomously optimize to I/O 
 * subsystem when a well-known I/O pattern has occurred.
 * 
 * @section intrumentation Software Instrumentation
 * 
 * The are two ways an application can profit from SIOX. The easiest one is 
 * to simply compile the application against a SIOX-enabled library stack,
 * thereby harnessing SIOX support for these layers. The 
 * other one is to manually instrument the application using the SIOX API 
 * designed for that purpose. Before presenting some examples of manual source
 * code instrumentation, there are some fundamental concepts you should 
 * familiarize with.
 * 
 * @subsection nodes SIOX Nodes
 * 
 * A SIOX node is a logical functional unit, e.g. a hardware component 
 * or a software layer. It represent one of the conceptual links in the 
 * corresponding causal chain of I/O activities. A SIOX node is identified by 
 * the hardware it resides on (e.g. hostname), the software layer it implements
 * (e.g. application name or library name) and the application's instance
 * (e.g. process or thread id). This identification scheme is very flexible 
 * though and allows the combination of different data to form the different 
 * IDs as long as their concatenation can be used as a unique key. SIOX indexes 
 * all nodes using a @em UNID, a unique numeric key. Instrumented applications 
 * must register their SIOX nodes during startup as the first step.
 * 
 * @sa siox_register_node()
 * @sa siox_unregister_node()
 * 
 * @subsection activities SIOX Activities
 * 
 * A SIOX activity represents an I/O action taking place on a SIOX node. 
 * Activities can be nested and are created by wrapping one or more of the 
 * function calls that trigger I/O. They define the granularity at which the 
 * SIOX system will evaluate the I/O events on that particular node. At any  
 * layer, an activity will produce a cascade of sub-activities in the 
 * instrumented lower layers not stopping until the disk systems at the bottom 
 * of the cluster's I/O path. We call this group of related activities the 
 * @em causal @em chain. One of the key functionalities of SIOX is its 
 * ability to correlate these activities even in the presence of missing links 
 * (e.g. uninstrumented layers or components). This allows us to trace the I/O 
 * behavior observed at any point of the I/O path back to the application that
 * generated it.
 *  
 * @sa siox_start_activity();
 * @sa siox_stop_activity();
 * @sa siox_end_activity();
 * 
 * @subsection descriptors SIOX Descriptors
 * 
 * The information available to a given activity varies depending on the node 
 * it resides on. For example, while an activity at the application or POSIX 
 * layer knows the files it is accessing by name, an activity at the file system
 * layer may only know the involved @em inodes. Descriptors containing this 
 * context information are sent by the activity at the higher layer to the 
 * activities at the lower ones to ease the correlation of their file handlers
 * and other variables.
 * 
 * @subsection rcalls SIOX Remote Calls
 * 
 * (to be written)
 * 
 * @image html nodes-activities.png "SIOX Nodes, Activities and Descriptors"
 * 
 * @section interfaces SIOX Interfaces
 *
 * All interaction with SIOX is built on the low-level
 * interface @em siox-ll - right now, all examples shown use this API.
 * Later, the SIOX project will provide more comfortable and well-adapted
 * high-level interfaces for each of the types of nodes, such as @em siox-cache,
 * @em siox-network or @em siox-blockstorage.
 * Furthermore, wrappers may be used to easily adapt software already instrumented
 * for other tools to SIOX.
 * Both high-level APIs and wrappers will build upon @em siox-ll to perform their tasks.
 *
 * @image html siox-apis.png
 *
 * @section example Instrumentation Example
 *
 * The following example shows how some of the MPI-I/O functions could be 
 * instrumented.
 * 
 * @include mpi-example.c
 * @example mpi-example.c
 */



#ifndef siox_LL_H
#define siox_LL_H

#include "../ontology/ontology.h"


/**
 * The node's <em>Unique Node ID</em>.
 *
 * Identifies a node in the IOPm graph.
 */
typedef struct siox_unid_t * siox_unid;

/**
 * The <em>Activity ID</em>.
 *
 * Identifies an activity.
 */
typedef struct siox_aid_t * siox_aid;

/**
 * The <em>Remote Call ID</em>.
 *
 * Identifies a remote call and the group of attributes (e.g., parameters) associated
 * with it.
 */
typedef struct siox_rcid_t * siox_rcid;

/**
 * A time stamp, as represented in SIOX.
 */
typedef siox_timestamp_t siox_timestamp;



/**
 * @name Nodes
 * Nodes represent entities in the IOPm graph, e.g., a software layer such as HDF5 or
 * a hardware component such as a block storage device.
 *
 * In SIOX, each node is identified by a <em>Unique Node ID (UNID)</em> which is assigned
 * upon registration.
 *
 * A node's typical life cycle would consist of:
 * <ol>
 * <li>Checking in with SIOX in its initalisation function via siox_register_node().</li>
 * <li>Also there, reporting any additional attributes via siox_node_attribute().</li>
 * <li>Performing its duties as usual.</li>
 * <li>Checking out with SIOX in its finalization function via siox_unregister_node().</li>
 * </ol>
 */
/**@{*/

/**
 * Register this node with SIOX and assign a fresh @em UNID to it.
 *
 * SIOX will use the information given to create a new node in its IOPm model of the system.
 *
 * @param[in]   hwid    The node's @em HardwareID, e.g. „Blizzard Node 5“ or the MacID
 *                      of an NAS hard drive.
 * @param[in]   swid    The node's @em SoftwareID, e.g. „MPI“ oder „POSIX“.
 * @param[in]   iid     The node's @em InstanceID (according to node type, e.g. the ProcessID
 *                      of a Thread or the IP address and Port od a server cache).
 *
 * @return  A fresh @em UNID to be used in all the node's future communications with SIOX.
 *
 * @note Any parameter SIOX can find out on its own (such as a ProcessID) may be @c NULL.
 */
siox_unid siox_register_node(const char * hwid, const char * swid, const char * iid);

/**
 * Unregister a node with SIOX.
 *
 * @param[in]   unid    The @em UNID  SIOX had assigned to this node.
 */
void siox_unregister_node(siox_unid unid);

/**
 * Report further attributes of this node to SIOX.
 *
 * @em Example:    A cache reportin its capacity as 10,000 bytes.
 *
 * @param[in]   unid        The node's @em UNID.
 * @param[in]   dtid        The @em DTID of the attribute.
 * @param[in]   value       A pointer to the attribute's value.
 */
void siox_node_attribute(siox_unid unid, siox_dtid dtid, const void * value);

/**@}*/




/**
 * @name Activities
 * Activities bundle a series of actions so that the can be related to one or more
 * performance metrics of the system influenced by these actions.
 *
 * As an example, an activity might bundle calls to open a file, write to it and close it again.
 * This way, the performance metrics of maximum throughput and average write speed can e linked
 * to this chain of actions.
 *
 * When defining an activity, the usual course of action is as follows:
 * <ol>
 * <li>Start the activity in its active phase and receive @em AID via siox_start_activity().</li>
 * <li>Execute usual commands, calls, etc. pertaining to activity.</li>
 * <li>End active phase and switch to reporting phase via siox_stop_activity().</li>
 * <li>Gather any performance data associated with the activity and report it
 * via siox_repot_activity().</li>
 * <li>Close the activity via siox_end_activity().</li>
 * </ol>
 * If no reporting is to be done in step 4, steps 3 may be omitted as well.
 */
/**@{*/

/**
 * Report the start of an activity and receive an <em>Activity ID (AID)</em> for it.
 *
 * This will mark the time stamp given as the beginning of the activity, influencing any
 * performance metrics associated that are measured over time.
 *
 * SIOX will use the @em AID to correctly assign attributes used and performance metrics influenced
 * by this activity.
 * As any @em AID is linked to its node by SIOX, functions supplied with an @em AID do not use a @em UNID.
 * 
 *
 * @param[in]   unid        The node's @em UNID.
 * @param[in]   timestamp   A time stamp or @c NULL,
 *                          which will result in SIOX using the current time.
 * @param[in]   comment     Possible details about the activity, or @c NULL.
 * @return                  A fresh @em AID, to be used in all the activity's further dealings with SIOX.
 */
siox_aid siox_start_activity(siox_unid unid, siox_timestamp * timestamp, const char * comment);

/**
 * Report the end of an activity's active phase, beginning its reporting phase.
 *
 * This will mark the time stamp given as the end of the activity, influencing any
 * performance metrics associated that are measured over time.
 *
 * @param[in]   aid         The activity's @em AID.
 * @param[in]   timestamp   A time stamp or @c NULL,
 *                          which will result in SIOX using the current time.
 * @note    If there are no metrics to report for an activity, siox_stop_activity() can be omitted.
 */
void siox_stop_activity(siox_aid aid, siox_timestamp * timestamp);

/**
 * Report performance data to be associated with the activity.
 *
 * @em Example:   After being called to write a number of bytes to block storage, the node reports
 *                the number of bytes successfully written, the average cache fill level oder the
 *                maximum throughput achieved.
 *
 * @param[in]   aid             The activity's @em AID.
 * @param[in]   mid             The metric's @em MID.
 * @param[in]   value           A pointer to the metric's actual value.
 */
void siox_report_activity(siox_aid aid, siox_mid mid, void * value);

/**
 * Report that the current call resulted in the error code @em error so that SIOX can mark any
 * performance metrics gathered accordingly.
 * This should be done as part of any regular error processing by calls issued by the activity.
 *
 * @param[in]   aid     The activity's @em AID.
 * @param[in]   error   The error code returned by the function.
 */
void siox_report_error(siox_aid aid, int error);

/**
 * Report the end of an activity's report phase and close it.
 *
 * After calling this function, the behaviour of further calls to siox_report_activity()
 * with the same @em AID will be undefined.
 *
 * If no performance metrics were to be reported, the usual prior call to siox_stop_activity()
 * may be omitted, in which case timstamp will be used to determine the end of the activity's
 * active phase. Otherwise, timestamp is discarded.
 *
 * @param[in]   aid         The activity's @em AID.
 * @param[in]   timestamp   A time stamp or @c NULL,
 *                          which will result in SIOX using the current time.
 */
void siox_end_activity(siox_aid aid, siox_timestamp * timestamp);

/**
 * Causally link an activity to another.
 *
 * This will enable SIOX to relate the activities' influence on system performance.
 * While functions directly called by other functions can be linked by SIOX automatically,
 * the relation between two calls related "horizontally" (such as one to open a file
 * and one to write to it) is hard to impossible to determine automatically.
 * This function will give SIOX the information necessary to do so.
 *
 * @param[in]   aid_child   The @em AID of the current activity.
 * @param[in]   aid_parent  The @em AID of an activity causally preceding the current one.
 */
void siox_link_activity(siox_aid aid_child, siox_aid aid_parent);

/**
 * Report performance data @em not associated with a single activity.
 *
 * @em Example:   Every second, the node reports its average processor load, maximum memory usage
 *                and total idle time to SIOX.
 *
 * @param[in]   unid    The node's @em UNID.
 * @param[in]   mid     The metric's @em MID.
 * @param[in]   value   A pointer to the metric's actual value.
 */
void siox_report(siox_unid unid,  siox_mid  mid, void * value);

/**@}*/



/**
 * @name Remote Calls
 * Remote calls allow SIOX to reconstruct causal relations of caller and callee
 * via physical system boundaries.
 *
 * While locally, SIOX can follow caller-callee relations by itself, for calls crossing
 * physical system boundaries, this is much harder.
 * The path chosen here requires both caller and callee to report to SIOX the attributes
 * of the call (such as key parameters passed) to allow for association by best match.
 *
 * The usual sequence is as follows:
 * <ul>
 * <li>On caller's side:</li>
 * <ol>
 * <li>Open attribute list and indicate target via siox_describe_remote_call_begin().</li>
 * <li>Report attributes to be passed via siox_describe_remote_call_attribute().</li>
 * <li>Close attribute list via siox_describe_remote_call_end().</li>
 * <li>Call callee.</li>
 * </ol>
 * <li>On callee's side:</li>
 * <ol>
 * <li>Report attributes received via siox_remote_call_receive().</li>
 * <li>Execute usual code.</li>
 * </ol>
 * </ul>
 */
/**@{*/

/**
 * Open attribute list for a remote call, indicate its target and receive @em RCID.
 *
 * @param[in]   aid           The current activity's @em AID.
 * @param[in]   target_hwid   The target node's @em HardwareID (e.g. „Blizzard Node 5“ or the MacID
 *                            of an NAS hard drive), if known; otherwise, @c NULL.
 * @param[in]   target_swid   The target node's @em SoftwareID (e.g. „MPI“ oder „POSIX“),
 *                            if known; otherwise, @c NULL.
 * @param[in]   target_iid    The target node's @em InstanceID (according to node type,
 *                            e.g. the ProcessID of a Thread or the IP address and Port
 *                            of a server cache), if known; otherwise, @c NULL.
 *
 * @return                    A fresh @em RCID to be used in all the remote call's
 *                            future communications with SIOX.
 */
siox_rcid siox_describe_remote_call_start(siox_aid      aid,
                                          const char *  target_hwid, 
                                          const char *  target_swid,
                                          const char *  target_iid);

/**
 * Report an attribute to be sent via a remote call.
 *
 * @param[in]   rcid    The remote call's @em RCID.
 * @param[in]   dtid    The attribute's @em DTID.
 * @param[in]   value   The attribute's actual value.
 */
void siox_remote_call_attribute(siox_rcid rcid, siox_dtid dtid, void * value);

/**
 * Close attribute list for a remote call.
 *
 * @param[in]   rcid    The remote call's @em RCID.
 */
void siox_describe_remote_call_end(siox_rcid  rcid);

/**
 * Report the reception of an attribute via a remote call.
 *
 * @param[in]   aid     The current activity's @em AID.
 * @param[in]   dtid    The attribute's @em DTID.
 * @param[in]   value   The attribute's actual value.
 */
void siox_remote_call_receive(siox_aid aid, siox_dtid dtid, void * value);

/**@}*/


/**
 * @name Accessing the Ontology
 */
/**@{*/

/**
 * Set the ontology to be used.
 * This function has to be called before any functions using the ontology, such as all
 * siox_register_...() calls.
 * Omitting it will result in SIOX accessing the default ontology.
 *
 * @param[in]   name    The name of the ontology to be used.
 *
 * @returns             @c true, if the ontology was set successfully; @c false, otherwise.
 */
bool siox_set_ontology( const char * name );

/**
 * Find the @em DTID for the data type with the name given.
 * If it already exists in the ontology (only the name is checked to test for this!), return its @em DTID;
 * otherwise, create it and return the fresh @em DTID.
 *
 * @param[in]   name        The data type's unique name.
 * @param[in]   storage     The minimum storage type required to store data of the data type.
 *                          This type will also to be assumed for type casts!
 *
 * @returns                 The @em DTID of the data type.
 */
siox_dtid siox_register_datatype( const char * name, enum siox_ont_storage_type storage );

/**
 * Prepare SIOX to treat a given datatype as a descriptor.
 *
 * Descriptors are attributes, often parameters passed in a call, that identify entities on the
 * data path through the I/O stack, helping SIOX to reconstruct causal chains and dependencies.
 * Examples for descriptors are file names, file handles and logical block addresses.
 *
 * @param[in]   dtid            The @em DTID of the datatype concerned.
 */
void siox_register_datatype_as_descriptor(siox_dtid dtid);

/**
 * Find the @em MID for the metric with the name given.
 * If it already exists in the ontology (only the name is checked to test for this!), return its @em MID;
 * otherwise, create it and return the fresh @em MID.
 *
 * @param[in]   name        The metric's unique name.
 * @param[in]   unit        The unit used to measure values in the metric.
 * @param[in]   storage     The minimum storage type required to store data of the metric.
 *                          This type will also be assumed for type casts!
 * @param[in]   scope       The way in which the data collected was derived.
 *
 * @returns                 The @em MID of the metric.
 */
siox_mid siox_register_metric( const char *                 name,
                               enum siox_ont_unit_type      unit,
                               enum siox_ont_storage_type   storage,
                               enum siox_ont_scope_type     scope );

/**@}*/

#endif
