/**
 * @file siox-ll.h SIOX low-level interface headers.
 *
 * @date 03/2013
 * @copyright GNU Public License
 * @authors Michaela Zimmer, Julian Kunkel, Marc Wiedemann & Alvaro Aguilera
 */



#ifndef siox_LL_H
#define siox_LL_H

#include <monitoring/datatypes/ids.hpp>
#include <monitoring/ontology/ontology.h>


/*
 * ==================================
 * A note on storage mangement:
 * ----------------------------------
 * Right now, our philosophy is that SIOX will take care of
 * all objects created by calls to it.
 * Thus, instrumenting a component for SIOX will not
 * burden you with calls to malloc() and free(); the library
 * will do it all for you.
 * ==================================
 */

/**
 * A component, as represented in SIOX.
 */
typedef struct siox_component siox_component;

/**
 * An activity, as represented in SIOX.
 */
typedef struct siox_activity siox_activity;

/**
 * A remote call, as represented in SIOX.
 */
typedef struct siox_remote_call siox_remote_call;

/**
 * A time stamp, as represented in SIOX.
 */
typedef uint64_t * siox_timestamp;



/**
 * Register a process (program, could be a server as well)
 * This will be done automatically by register_component() if not provided. 
 * 
 * @param[in]   hwid    identifies the hardware the process is running on. e.g. „Blizzard Node 5“ or the MacIDof an NAS hard drive.
 * 
 * If it is null it will be looked up automatically.
 * pid identifies the process.
 * If it is null it will be looked up automatically.
 * 
 * Subsequent calls to this function will ignore the pid and hwid parameters but add new attributes.
 * Rationales:
 * 1) Multiple calls to this function must be possible to simplify usage in applications.
 * 2) Explicit setting of values is used for testing.
 */
void siox_process_register(HwID * hwid, ProcessID * pid);

/**
 Adding of attributes allows different layers to add global valid runtime information (such as MPI rank).
 Although it is possible to realize this with attributes attached to individual components.
 */
void siox_process_set_attribute(siox_attribute * attribute, void * value);

/*
 * This call indicates the completion of the application.
 * SIOX might cleanup internal datastructures and/or flush information along the monitoring and knowledge paths.
 * Subsequent calls to any SIOX interface should be avoided.
 */
void siox_process_finalize();


/**
 * Register an attribute type with SIOX.
 *
 * In contrast to metrics, attributes are not supposed to be globally unique - their semantics
 * will vary with the entity they apply to.
 */
//@test ''%s,%d'' name,storage_type
siox_attribute * siox_attribute_register(const char * name, enum siox_ont_storage_type storage_type);

/**@}*/

/**
 * @name Components
 * Components represent entities in the IOPm graph, e.g., a software layer such as HDF5 or
 * a hardware component such as a block storage device.
 *
 * A component's typical life cycle would consist of:
 * <ol>
 * <li>Checking in with SIOX in its initalisation function via siox_component_register().</li>
 * <li>Also there, reporting any additional attributes via siox_component_set_attribute().</li>
 * <li>Performing its duties as usual.</li>
 * <li>Checking out with SIOX in its finalization function via siox_component_unregister().</li>
 * </ol>
 */
/**@{*/

/**
 * Register this component with SIOX.
 *
 * SIOX will use the information given to create a new component in its IOPm model of the system.
 *
 * @param[in]   uid    The component's @em SoftwareInterface identifier, e.g. „MPI2“ oder „POSIX“.
 * @param[in]   iid     The component's @em InstanceID (according to component type, e.g.
 *                      the ProcessID of a Thread or the IP address and Port od a server cache).
 * These arguments provide a human readable name for the software layer and are used for matching of remote calls.
 * 
 * @return  A fresh @em siox_component to be used in all the component's future communications
 *          with SIOX.
 *
 * @note Any parameter SIOX can find out on its own (such as a ProcessID) may be @c NULL.
 */
//@test ''%s-%s-%s'' hwid,swid,iid
siox_component * siox_component_register(UniqueInterfaceID * uid, const char * iid);

/**
 * Report an attribute of this component to SIOX.
 *
 * @em Example:    A cache reporting its capacity as 10,000 bytes.
 *
 * @param[in]   component   The component.
 * @param[in]   attribute   The attribute to be set.
 * @param[in]   value       A pointer to the attribute's value.
 */
//@test ''%p,%p,%p'' component,attribute,value
void siox_component_set_attribute(siox_component * component, siox_attribute * attribute, void * value);

/**
 * Register an attribute as a descriptor for activities (!) of this component.
 *
 * @em Example:    POSIX registering the attribute type "file handle" to serve as descriptor.
 *
 * @param[in]   component   The component.
 * @param[in]   attribute   The attribute.
 */
//@test ''%p,%p'' component,attribute
void siox_component_register_descriptor(siox_component * component, siox_attribute * attribute);

/**
 * Retrieve a named attribute of a component.
 *
 * @param[in]   component   The component.
 * @param[in]   name        The name of the attribute to be retrieved.
 */
//@test ''%p,%s'' component,name
siox_attribute * siox_component_get_attribute(siox_component * component, const char * name);

/**
 * Unregister a component with SIOX.
 *
 * @param[in]   component   The component.
 */
//@test ''%p' component
void siox_component_unregister(siox_component * component);


/**@}*/




/**
 * @name Activities
 * Activities bundle a series of actions so that they can be related to one or more
 * performance metrics of the system influenced by these actions.
 *
 * As an example, an activity might bundle calls to open a file, write to it and close it again.
 * This way, the performance metrics of maximum throughput and average write speed can be linked
 * to this chain of actions.
 *
 * When defining an activity, the usual course of action is as follows:
 * <ol>
 * <li>Start the activity in its active phase via siox_activity_start().</li>
 * <li>Execute any usual commands, calls, etc. pertaining to activity.</li>
 * <li>End active phase and switch to reporting phase via siox_activity_stop().</li>
 * <li>Gather any performance data associated with the activity and report it
 * via siox_activity_report().</li>
 * <li>Close the activity via siox_activity_end().</li>
 * </ol>
 */
/**@{*/

/**
 * Report the start of an activity.
 *
 * This will mark the time stamp given as the beginning of the activity, influencing any
 * performance metrics associated that are measured over time.
 *
 * SIOX will use the @em siox_activity to correctly assign attributes used and performance
 * metrics influenced by this activity.
 * As any activity is linked to its component by SIOX, functions supplied with a
 * @em siox_activity do not use a @em siox_component.
 *
 *
 * @param[in]   component   The component.
 * @param[in]   timestamp   A time stamp or @c NULL,
 *                          which will result in SIOX using the current time.
 * @param[in]   comment     Possible details about the activity, or @c NULL.
 *                          This is meant for plain text comments only.
 * @return                  A fresh @em siox_activity, to be used in all the activity's
 *                          further dealings with SIOX.
 */
//@test ''%p'' component
siox_activity * siox_activity_start(siox_component * component, siox_timestamp timestamp, const char * comment);

/**
 * Report the end of an activity's active phase, beginning its reporting phase.
 *
 * This will mark the time stamp given as the end of the activity, influencing any
 * performance metrics associated that are measured over time.
 *
 * @param[in]   activity    The activity.
 * @param[in]   timestamp   A time stamp or @c NULL,
 *                          which will result in SIOX using the current time.
 */
//@test ''%p,%p'' activity,timestamp
void siox_activity_stop(siox_activity * activity, siox_timestamp timestamp);

/**
 * Report an attribute of an activity.
 *
 * Attributes for activities are either its parameters or other values computed from them.
 * Metrics or statistics resulting from the call use siox_activity_report() instead.
 *
 * @param[in]   activity    The activity.
 * @param[in]   attribute   The attribute.
 * @param[in]   value       A pointer to the attribute's value.
 */
//@test ''%p,%p,%p'' activity,attribute,value
void siox_activity_set_attribute(siox_activity * activity, siox_attribute * attribute, void * value);

/**
 * Retrieve an attribute of an activity.
 *
 * @param[in]   activity    The activity.
 * @param[in]   name        The name of the attribute to be retrieved.
  */
//@test ''%p,%s'' activity,name
siox_attribute * siox_activity_get_attribute(siox_activity * activity, const char * name);

/**
 * Report performance data to be associated with the activity.
 *
 * @em Example:     After being called to write a number of bytes to block storage,
 *                  the component reports the number of bytes successfully written,
 *                  the average cache fill level oder the maximum throughput achieved.
 *
 * @param[in]   activity        The activity.
 * @param[in]   metric          The metric describing the data.
 * @param[in]   value           A pointer to the metric's actual value.
 */
//@test ''%p,%p,%p'' activity,metric,value
void siox_activity_report(siox_activity * activity, siox_metric * metric, void * value);

/**
 * Report that the current call resulted in the error code @em error so that SIOX can mark any
 * performance metrics gathered accordingly.
 * This should be done as part of any regular error processing by calls issued by the activity.
 *
 * @param[in]   activity    The activity.
 * @param[in]   error       The error code returned by the function.
 */
//@test ''%p,%d'' activity,error
void siox_activity_report_error(siox_activity * activity, int error);

/**
 * Mark the end of an activity's report phase and close it.
 *
 * After calling this function, the behaviour of further calls to siox_activity_report()
 * with the same activity will be undefined.
 *
 * @param[in]   activity    The activity.
 */
//@test ''%p'' activity
void siox_activity_end(siox_activity * activity);

/**
 * Causally link an activity to another.
 *
 * This will enable SIOX to relate the activities' influence on system performance.
 * While functions directly called by other functions can be linked by SIOX automatically,
 * the relation between two calls related "horizontally" (such as one to open a file
 * and one to write to it) is hard to impossible to determine automatically.
 * This function will give SIOX the information necessary to do so.
 *
 * @param[in]   activity_child   The current activity.
 * @param[in]   activity_parent  An activity causally preceding the current one.
 */
//@test ''%p,%p'' activity_child,activity_parent
void siox_activity_link_to_parent(siox_activity * activity_child, siox_activity * activity_parent);

/**
 * Report performance data @em not associated with a single activity.
 *
 * @em Example:   Every second, the component reports its average processor load,
 *                maximum memory usage and total idle time to SIOX.
 *
 * @param[in]   component   The component.
 * @param[in]   metric     The metric.
 * @param[in]   value   A pointer to the metric's actual value.
 */
//@test ''%p,%p,%p'' component,metric,value
void siox_report(siox_component * component,  siox_metric * metric, void * value);

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
 * <li>Open attribute list and indicate target via siox_remote_call_start().</li>
 * <li>Report attributes to be passed via siox_remote_call_attribute().</li>
 * <li>Close attribute list via siox_remote_call_execute().</li>
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
 * @param[in]   component     The component.
 * @param[in]   target_hwid   The target component's @em HardwareID (e.g. „Blizzard Node 5“ or the MacID
 *                            of an NAS hard drive), if known; otherwise, @c NULL.
 * @param[in]   target_uid   The target component's @em Software interface (e.g. „MPI“ oder „POSIX“),
 *                            if known; otherwise, @c NULL.
 * @param[in]   target_iid    The target component's @em InstanceID (according to component type,
 *                            e.g. the ProcessID of a Thread or the IP address and Port
 *                            of a server cache), if known; otherwise, @c NULL.
 *
 * @return                    A fresh @em RCID to be used in all the remote call's
 *                            future communications with SIOX.
 */
//@test ''%p,%s-%s-%s'' component,target_hwid,target_swid,target_iid
siox_remote_call * siox_remote_call_start(siox_component *  component,
                                          HwID 				* target_hwid,
                                          UniqueInterfaceID * target_uid,
                                          const char 		* target_iid);

/**
 * Report an attribute to be sent via a remote call.
 *
 * @param[in]   remote_call    The remote call.
 * @param[in]   attribute   The attribute.
 * @param[in]   value       A pointer to the attribute's value.
 */
//@test ''%p,%p,%p'' remote_call,attribute,value
void siox_remote_call_set_attribute(siox_remote_call * remote_call, siox_attribute * attribute, void * value);

/**
 * Close attribute list for a remote call.
 * This is necessary as there may be various remote calls originating from a single activity.
 *
 * @param[in]   remote_call    The remote call.
 */
//@test ''%p'' remote_call
void siox_remote_call_execute(siox_remote_call * remote_call);

/**
 * Report the reception of an attribute via a remote call.
 * While during a single activity, many remote calls may be sent, only one (the one initiating
 * the current function invocation) may be received. Hence, on the callee's side, the delimiting
 * functions siox_remote_call_start() and siox_remote_call_execute() are not needed.
 *
 * @param[in]   component   The component.
 * 
 * Optional parameters identify the callee if possible.
 * 
 */ 
//@test ''%p,%p,%p'' component,attribute,value
void siox_remote_call_received(siox_component * component, 	HwID * hwid, UniqueInterfaceID * uuid, const char * instance);

/**@}*/

/**
 * Register a metric with SIOX.
 *
 * If no metric with the name given is found in the ontology, a new entry will be created.
 * If a metric with the same name but inconsistent data is found, @c NULL will be returned.
 *
 * @param[in]   ontology    The ontology.
 * @param[in]   name        The metric's full qualified name.
 * @param[in]   unit        The unit used to measure values in the metric.
 * @param[in]   storage     The minimum storage type required to store data of the metric.
 *                          This type will also be assumed for type casts!
 * @returns                 The @em siox_metric for the metric or NULL, if the data given
 *                          is inconsistent with an existing metric of the same name..
 */
//@test ''%p,%s,%s,%d,%d'' ontology,name,unit,storage,scope
siox_metric * siox_ontology_register_metric(const char *                canonical_name,
                                            const char *                unit,
                                            enum siox_ont_storage_type  storage);
/* Attributes describe a metrics further:
                          e.g. "component" = "network", "derivation_rules" = "time / size", "type" = "throughput"
*/
void siox_metric_set_attribute(siox_metric * metric, siox_attribute * attribute, void * value);


/**
 * Retrieve a metric by its name. TODO: use attributes to retrieve it.
 *
 * @param[in]   ontology    The ontology.
 * @param[in]   name        The metric's full qualified name.
 *
 * @returns                 The @em siox_metric with the name given or NULL, if no such metric
 *                          could be found.
 */
//@test ''%p,%s'' ontology,name
siox_metric * siox_ontology_find_metric_by_name( const char * canonical_name);

/**@}*/

#endif
