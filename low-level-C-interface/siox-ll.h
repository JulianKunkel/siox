/**
 * @file    siox-ll.h
 *          Headerdatei for the SIOX-Low-Level-Interface
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2012
 *          GNU Public License
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
 * @return                  A fresh @em AID, to be used in all the activity's further dealings with SIOX.
 */
siox_aid siox_start_activity(siox_unid unid, siox_timestamp * timestamp);

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
