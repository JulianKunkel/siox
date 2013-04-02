template = {
# component
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
#
# SWID: The name (software id) for this component
'component': {
	'variables': 'SWID',
	'global': '''siox_component * global_component;
				 siox_ontology * global_ontology;
                 char global_pid;
                 char hostname[1024];
				''',
    'init': '''hostname[1023] = '\\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%%d", getpid() );
               global_component = siox_component_register(hostname, %(SWID)s, &global_pid);
               global_ontology = siox_ontology_connect();''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'siox_component_unregister(global_component);'
},
# register_attribute
#
# Registers a new attribute type with siox, so that later calls
# can set values for this type of attribute on certain entities.
#
# AttributeVariable: Name of the variable which is used to store the
#		   			 pointer to the attribute type
# Name: Name of the attribute
# MinStorage: The minimum storage type required to store this
#			  attribute
'register_attribute': {
	'variables': 'AttributeVariable Name MinStorage',
	'global': '''siox_attribute * %(AttributeVariable)s;''',
	'init': '''%(AttributeVariable)s = siox_attribute_register( %(Name)s, %(MinStorage)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# register_descriptor
#
# Registers a new descriptor type with siox, so that later calls
# can set values for this type of descriptor on certain entities.
#
# DescriptorVariable: Name of the variable which is used to store the
#		   			 pointer to the descriptor type
# Name: Name of the descriptor
# MinStorage: The minimum storage type required to store this
#			  descriptor
'register_descriptor': {
	'variables': 'DescriptorVariable Name MinStorage',
	'global': '''siox_attribute * %(DescriptorVariable)s;''',
	'init': '''%(DescriptorVariable)s = siox_attribute_register( %(Name)s, %(MinStorage)s );
			   siox_component_register_descriptor( global_component, %(DescriptorVariable)s );''',

    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# component_attribute
#
# Reports the value of one of the component's attributes to SIOX
#
# Attribute: Attribute type to set; must have been registered via
#			 siox_attribute_register().
# Value: Pointer to the real value of the attribute
'component_attribute': {
	'variables': 'Attribute Value',
	'global': '''''',
	'init': '''siox_component_set_attribute( global_component,  %(Attribute)s, %(Value)s);''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_create_str
#
# Creates a map data structure to keep track of descriptors that can be represented as strings.
# These will be used for the horizontal linking of all activities using that descriptor via
# activity_link_str.
#
# MapName: The name for this map; defaults to activityHashTable_str
'horizontal_map_create_str': {
	'variables': 'MapName=activityHashTable_str',
	'global': '''GHashTable * %(MapName)s;''',
    'init': '''%(MapName)s = g_hash_table_new(g_str_hash, g_str_equal);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_create_int
#
# Creates a map data structure to keep track of descriptors that can be represented as
# either ints or long ints.
# These will be used for the horizontal linking of all activities using that descriptor via
# activity_link_int.
#
# MapName: The name for this map; defaults to activityHashTable_int
'horizontal_map_create_int': {
	'variables': 'MapName=activityHashTable_int',
	'global': '''GHashTable * %(MapName)s;''',
    'init': '''%(MapName)s = g_hash_table_new(g_direct_hash, g_direct_equal);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
# register_metric
#
# Registers a new metric with SIOX.
#
# MetricVariable: Name of the variable to store the metric type in
# Name: Name of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'register_metric': {
	'variables': 'MetricVariable Name UnitType StorageType ScopeType',
	'global': '''siox_metric * %(MetricVariable)s;''',
	'init': '''%(MetricVariable)s = siox_ontology_register_metric( global_ontology, %(Name)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# activity
#
# Starts (at the beginning) and stops (at the end) a new
# activity in the current function.
# Any metrics resulting from the activity still need to be reported via activity_report!
#
# Name: Short description of the activity
# Activity: Name of the variable to store the activity in; defaults to sioxActivity
# TimeStart: Start time to be reported; defaults to NULL, which will draw a current time stamp
# TimeStop: Stop time to be reported; defaults to NULL, which will draw a current time stamp
'activity': {
	'variables': 'Name=__FUNCTION__ Activity=sioxActivity TimeStart=NULL TimeStop=NULL',
	'global': '''''',
	'init': '''''',
    'before': '''siox_activity * %(Activity)s = siox_activity_start( global_component, %(TimeStart)s, %(Name)s );''',
	'after': '''siox_activity_stop( %(Activity)s, %(TimeStop)s );''',
	'cleanup': 'siox_activity_end( %(Activity)s );',
	'final': ''
},
# activity_attribute
#
# Tie an attibute to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead.
#
# Attribute: The attribute type to be reported
# Value: The actual value to be reported
# Activity: The activity; defaults to sioxActivity
'activity_attribute': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''siox_activity_set_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# activity_descriptor
#
# Tie an attibute serving as a descriptor to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead.
#
# Attribute: The attribute type to be reported
# Value: The actual value to be reported
# Activity: The activity; defaults to sioxActivity
'activity_descriptor': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'global': '''''',
	'init': '''siox_component_register_descriptor( global_component, %(Attribute)s );''',
    'before': '''siox_activity_set_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_put_int
#
# Tie an attibute serving as a descriptor to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
# Activity: The activity; defaults to sioxActivity
'horizontal_map_put_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''g_hash_table_insert( %(MapName)s, GINT_TO_POINTER(%(Key)s), %(Activity)s );''',
	'cleanup': '',
	'final': ''
},
# horizontal_map_put_str
#
# Tie an attibute serving as a descriptor to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as a string
# MapName: The map to be used; defaults to activityHashTable_int
# Activity: The activity; defaults to sioxActivity
'horizontal_map_put_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''g_hash_table_insert( %(MapName)s, %(Key)s, %(Activity)s );''',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_int
#
# Clear an attibute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_int.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''g_hash_table_remove( %(MapName)s, GINT_TO_POINTER(%(Key)s) );''',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_str
#
# Clear an attibute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_str.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''g_hash_table_remove( %(MapName)s, %(Key)s );''',
	'cleanup': '',
	'final': ''
},
# activity_link_int
#
# Horizontally links the current activity (started with activity) to another one via
# a desctriptor represented as either an int or a long int.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in int or long int form
# Activity: Activity to be linked; defaults to sioxActivity
'activity_link_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''siox_activity * Parent = (siox_activity*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) );
    			siox_activity_link_to_parent( %(Activity)s, Parent );
			  ''',
	'cleanup': '',
	'final': ''
},
# activity_link_str
#
# Horizontally links the current activity (started with activity) to another one via
# a desctriptor represented as a string.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in string form
# Activity: Activity to be linked; defaults to sioxActivity
'activity_link_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''siox_activity * Parent = (siox_activity*) g_hash_table_lookup( %(MapName)s, %(Key)s );
    			siox_activity_link_to_parent( %(Activity)s, Parent );
			  ''',
	'cleanup': '',
	'final': ''
},
# activity_report
#
# Reports a metric tied to an activity
#
# Metric: Metric type of the value to be reported; must be registered with register_metric!
# Value: The value to be reported
# Activity: The activity; defaults to sioxActivity
'activity_report': {
	'variables': 'Metric Value Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_activity_report( %(Activity)s, %(Metric)s, (void *) &%(Value)s );',
	'cleanup': '',
	'final': ''
},
# error
#
# Reports that an activity terminated with an error
#
# Condition: A guard expression to be evaluated after the wrapped function call;
#			 if true, an error with code Error will be reported to SIOX, indicating
#			 that all metrics monitored on this call are tainted.
# Error: The error code to be reported; must be of type int
# Activity: The Activity to be reported; defaults to sioxActivity
'error': {
	'variables': 'Condition Error Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''if ( %(Condition)s )
					siox_activity_report_error( %(Activity)s, %(Error)s );''', 
	'cleanup': '',
	'final': ''
},
# report
#
# Reports a metric that cannot be tied directly an activity
#
# Metric: Metric type of the value to be reported
# Value: The value to be reported
'report': {
	'variables': 'Metric Value',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report( global_component, %(Metric)s, (void *) &%(Value)s );',
	'cleanup': '',
	'final': ''
},
# remote_call_start
#
# Initiates a new remote call
#
# RemoteCallVariable: The name of the variable to store the remote call structure in
# TargetHWID: The hardware id of the target machine
# TargetSWID: The software id of the target machine
# TargetIID: The instance id of the target machine
'remote_call_start': {
	'variables': 'RemoteCallVariable TargetHWID TargetSWID TargetIID',
	'global': '''''',
	'init': '''siox_remote_call * %(RemoteCallVariable)s;\n''',
    'before': '''%(RemoteCallVariable)s = siox_remote_call_start( global_component, %(TargetHWID)s, %(TargetSWID)s, %(TargetIID)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_attribute
#
# Attach an attribute to a remote call
#
# RemoteCall: The corresponding remote call
# Attribute: The attribute type of the value
# Value: The actual value to be sent
'remote_call_attribute': {
	'variables': 'RemoteCall Attribute Value',
	'global': '''''',
	'init': '''''',
    'before': '''siox_remote_call_set_attribute( %(RemoteCall)s, %(Attribute)s, (void *) &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_descriptor
#
# Attach an attribute serving as a descriptor to a remote call
#
# RemoteCall: The corresponding remote call
# Attribute: The attribute type of the value
# Value: The actual value to be sent
'remote_call_descriptor': {
	'variables': 'RemoteCall Attribute Value',
	'global': '''''',
	'init': '''siox_component_register_descriptor( global_component, %(Attribute)s );''',
    'before': '''siox_remote_call_set_attribute( %(RemoteCall)s, %(Attribute)s, (void *) &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_execute
#
# Closes a remote call's attribute list
#
# RemoteCall: The remote call to be closed
'remote_call_execute': {
	'variables': 'RemoteCall',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_remote_call_execute( %(RemoteCall)s );',
	'cleanup': '',
	'final': ''
},
# remote_call_received
#
# Reports an attribute of a remote call just received
#
# Attribute: The attribute type of the value received
# Value: The actual value received
# Activity: The activity; defaults to sioxActivity
'remote_call_received': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''siox_remote_call_received( global_component, %(Attribute)s, (void *) &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# splice_before
#
# Used to insert custom code before the call
#
# PROGRAMCODE: The code to insert
'splice_before': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '%(PROGRAMCODE)s',
	'after': '',
	'cleanup': '',
	'final': ''
},
# splice_after
#
# Used to insert custom code after the call
#
# PROGRAMCODE: The code to insert
'splice_after': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '%(PROGRAMCODE)s',
	'cleanup': '',
	'final': ''
},
# activity with hints
#
# Starts (at the beginning) and stops (at the end) a new
# activity in the current function and transfer the MPI hints to SIOX activity
# Any metrics resulting from the activity still need to be reported via activity_report!
#
# Name: Short description of the activity
# Activity: Name of the variable to store the activity in; defaults to sioxActivity
# TimeStart: Start time to be reported; defaults to NULL, which will draw a current time stamp
# TimeStop: Stop time to be reported; defaults to NULL, which will draw a current time stamp
'activity_with_hints': {
	'variables': 'Name=__FUNCTION__ Activity=sioxActivity TimeStart=NULL TimeStop=NULL',
	'global': '''''',
	'init': '''''',
	'before': '''siox_activity * %(Activity)s = siox_activity_start( global_component, %(TimeStart)s, %(Name)s );''',
	'after': '''siox_activity_tranfer_hints(%(Activity)s, info);
	siox_activity_stop( %(Activity)s, %(TimeStop)s );''',
	'cleanup': 'siox_activity_end( %(Activity)s );',
	'final': ''
},
# test
#
# Writes a given message to stdout.
#
# Text: The text to print
'test': {
    'variables': '''Text="" Text2=""''',
    'global': '''''',
    'init': '''''',
    'before': '''printf("%%s (%(Text)s);\\n", __FUNCTION__, %(Text2)s);''',
    'after': '',
    'cleanup': '',
    'final': ''
}
}

# Inserted before every call
forEachBefore = ""

# Inserted after every call
forEachAfter = ""

# Regexes for functions to throw away
throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

# Will be included
includes = ['<stdlib.h>', '<stdio.h>', '<siox-ll.h>', '<stdarg.h>', '<glib.h>']
