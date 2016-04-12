template = {

###############################################################################
# replay related
###############################################################################
'replay_datatype_splice_after': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'replay_datatype_after': '%(PROGRAMCODE)s'
},



'replay_wrapper_splice_before': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'replay_wrapper_before': '%(PROGRAMCODE)s'
},

'replay_wrapper_splice_after': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'replay_wrapper_after': '%(PROGRAMCODE)s'
},





###############################################################################
# SIOX related
###############################################################################

# component
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
# InterfaceName = Name of interface
# ImplementationIdentifier ?
# InstanceName ?
# ComponentVariable ? ?
# SpliceCode ? ? ?
'component': {
	'variables': 'InterfaceName ImplementationIdentifier InstanceName="" ComponentVariable=global SpliceCode=',
	'global': '',
    'init': '''''',
    'initLast': '',
	'final': '',
},

'autoInitializeLibrary':{
	'global' : "",
},
'createInitializerForLibrary':{
	'global' : "",
},
'callLibraryFinalize':{
	'after' : '',
},
'callLibraryFinalizeBefore':{
	'before' : '',
},
'callLibraryInitialize':{
	'before' : '',
},
# register_attribute
#
# Registers a new attribute type with siox, so that later calls
# can set values for this type of attribute on certain entities.
#
# AttributeVariable: Name of the variable which is used to store the
#		   			 pointer to the attribute type
# Name: Name of the attribute
# StorageType: The minimum storage type required to store this
#			  attribute
'register_attribute': {
	'variables': 'AttributeVariable Domain Name StorageType',
	'global': '',
	'init': '',
    'before': '',
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
'component_attribute_pointer': {
	'variables': 'Attribute Value SpliceCode=',
	'global': '',
	'after': '',
    'before': '',
	'cleanup': '',
	'final': ''
},
'component_attribute': {
	'variables': 'Attribute Value SpliceCode=',
	'global': '',
	'after': '',
    'before': '',
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
	'global': '',
    'init': '',
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
	'global': 'std::map<int,int> %(MapName)s;',
    'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
'horizontal_map_create_size': {
	'variables': 'MapName=activityHashTable_size',
	'global': '',
    'init': '',
	'before': '',
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
	'variables': 'Name=%(FUNCTION_NAME)s ComponentActivity=cv%(FUNCTION_NAME)s ComponentVariable=global ActivityVariable=sioxActivity',
	'global': '',
	'init': '',
    	'before': '',
	   'beforeLast': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
# This template allows to inject an activity into a component in which it belongs to.
'activityComponentSwitcher2BasedOnParent': {
	'variables': 'Name=%(FUNCTION_NAME)s Key=unknown ComponentVariable1=global ComponentVariable2=global MapName1=activityHashTable_int MapName2=activityHashTable_int2 ActivityVariable=sioxActivity ActivityParentVar=parent',
	'global': '',
	'init': '',

    	'before': '',
	   'beforeLast': '',
	'after': '',
	'cleanup': 'siox_activity_end( %(ActivityVariable)s );',
},
'guard': {
	'variables': 'Name=guard FC=%(FUNCTION_CALL)s ComponentVariable=global',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'cleanupLast': '\t}else{\n\t\t%(FC)s \t}',
	'final': ''
},
# activity_attribute
#
# Tie an attribute to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead.
#
# Attribute: The attribute type to be reported
# Value: The actual value to be reported
# Activity: The activity; defaults to sioxActivity
'activity_attribute': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': '',
},
'activity_attribute_pointer': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': '',
},
'activity_attribute_str': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': '',
},
'activity_attribute_u32': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': '',
},
'activity_attribute_late': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'after': '',
},
'activity_attribute_late_pointer': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'after': '',
},
'activity_attribute_late_u32': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'after': '',
},
# horizontal_map_put_int
#
# Tie an attribute serving as a descriptor to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
# Activity: The activity; defaults to sioxActivity
'horizontal_map_put_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '',
	'init': '''
	// get issued filehandle, increment and store association to map
	//int fd_%(MapName)s = issued;
	issued++;
	%(MapName)s[d->ret] = ret; 
	''',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
'horizontal_map_put_int_ID': {
	'variables': 'Key MapName=activityHashTable_int ActivityID=sioxActivityID',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
'horizontal_map_put_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_put_str
#
# Tie an attribute serving as a descriptor to an activity.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as a string
# MapName: The map to be used; defaults to activityHashTable_int
# Activity: The activity; defaults to sioxActivity
'horizontal_map_put_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_int
#
# Clear an attribute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_int.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
'horizontal_map_remove_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_str
#
# Clear an attribute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_str.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
    'after': '',
	'cleanup': '',
	'final': ''
},
# activity_link_int
#
# Horizontally links the current activity (started with activity) to another one via
# a descriptor represented as either an int or a long int.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in int or long int form
# Activity: Activity to be linked; defaults to sioxActivity
# The parent may be NULL in some cases
'activity_link_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '',
	'cleanup': '',
	'final': ''
},
'activity_link_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '',
	'init': '',
	'before': '''
		g_rw_lock_reader_lock(& lock_%(MapName)s);
		siox_activity_ID * Parent = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GSIZE_TO_POINTER(%(Key)s) );
		g_rw_lock_reader_unlock(& lock_%(MapName)s);
        siox_activity_link_to_parent( %(Activity)s, Parent ); 
			  ''',
	'cleanup': '',
	'final': ''
},

# This template allows to lookup a parent based on the integer key.
'activity_lookup_ID_int': {
	'variables': 'Key ActivityID=Parent MapName=activityHashTable_int',
	'after': '''g_rw_lock_reader_lock(& lock_%(MapName)s); 
		siox_activity_ID * %(ActivityID)s = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) ); 
		g_rw_lock_reader_unlock(& lock_%(MapName)s);''',
},
'activity_lookup_ID_before_int': {
	'variables': 'Key ActivityID=Parent MapName=activityHashTable_int',
   'before': '''g_rw_lock_reader_lock(& lock_%(MapName)s); 
		siox_activity_ID * %(ActivityID)s = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) ); 
		g_rw_lock_reader_unlock(& lock_%(MapName)s);''',
},
# Link an already known parent
'activity_link_parent': {
	'variables': 'Parent=Parent Activity=sioxActivity',
	'before': '',
},

# activity_link_str
#
# Horizontally links the current activity (started with activity) to another one via
# a descriptor represented as a string.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in string form
# Activity: Activity to be linked; defaults to sioxActivity
'activity_link_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '',
	'init': '',
    'before': '',
	'after': '',
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
	'variables': 'Condition="ret<0" Error=ret Activity=sioxActivity',
	'global': '',
	'init': '',
    	'before': '',
	'after': '''if ( %(Condition)s ){
                      siox_activity_report_error( %(Activity)s, %(Error)s );
                      siox_activity_stop(%(Activity)s);
                      siox_activity_end(%(Activity)s);
                      return ret;
		    }''',
	'cleanup': '',
	'final': ''
},
'errorErrno': {
        'variables': 'Condition="ret<0" Activity=sioxActivity',
        'global': '',
        'init': '',
        'before': '',
        'after': ''' int errsv = errno;
		    if ( %(Condition)s ){
                      siox_activity_report_error( %(Activity)s, errsv );
                      siox_activity_stop(%(Activity)s);
                      siox_activity_end(%(Activity)s);
							 errno = errsv;
							 return ret;
                    }''',
        'cleanup': '',
        #'cleanupLast': '''errno = errsv;''',        
        'final': ''
},
'restoreErrno': {
        'after': ''' errno = errsv; '''
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
	'variables': 'RemoteCallVariable activity TargetNode TargetUID TargetIID',
	'global': '',
	'init': '',
    'before': '',
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
	'global': '',
	'init': '',
    'before': '',
	'after': '',
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
	'variables': 'RemoteCall',
	'global': '',
	'init': '',
    'before': '',
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
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
'splice_final': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': '%(PROGRAMCODE)s'
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
	'after': '',
	'cleanup': '',
	'final': ''
},

'splice_once': {
	'variables': 'PROGRAMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},


'include': {
	'variables': 'what',
	'global': '',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
'print_fname': {
        'variables': '',
        'global': '',
        'init': '',
        'before': '',
        'after': '',
        'cleanup': '',
        'final': ''
},
'rewriteCall': { # This is a special template, interpreted by the skeleton-builder !
        'variables': 'functionName arguments= parameters=',
        'global': '',
        'init': '',
        'before': '',
        'after': '',
        'cleanup': '',
        'final': ''
}
}

templateParameters = {
# Insert global once
"globalOnce": "",
# Will be included
"includes" : ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<string.h>', '<stdint.h>']
}
