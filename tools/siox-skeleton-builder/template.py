template = {
# component
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
#
# SWID: The name (software id) for this component
'component': {
	'variables': 'InterfaceName ImplementationIdentifier InstanceName="" SpliceCode=',
	'global': '''static siox_component * global_component = NULL;
		     static siox_unique_interface * global_uid = NULL;
				''',
    'init': ''' if( global_component != NULL ) return;
    		  %(SpliceCode)s
    		  global_uid = siox_system_information_lookup_interface_id(%(InterfaceName)s, %(ImplementationIdentifier)s);
              global_component = siox_component_register(global_uid, %(InstanceName)s);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'if (global_component) { siox_component_unregister(global_component); }'
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
	'variables': 'AttributeVariable Domain Name StorageType',
	'global': '''static siox_attribute * %(AttributeVariable)s;''',
	'init': '''%(AttributeVariable)s = siox_ontology_register_attribute( %(Domain)s, %(Name)s, %(StorageType)s ); assert(%(AttributeVariable)s != NULL);''',
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
'component_attribute_pointer': {
	'variables': 'Attribute Value SpliceCode=',
	'global': '''''',
	'after': '''
				%(SpliceCode)s 
				assert( %(Attribute)s != NULL );
				siox_component_set_attribute( global_component,  %(Attribute)s, %(Value)s);
			''',
    'before': '''''',
	'cleanup': '',
	'final': ''
},
'component_attribute': {
	'variables': 'Attribute Value SpliceCode=',
	'global': '''''',
	'after': '''
				%(SpliceCode)s 
				assert( %(Attribute)s != NULL );
				siox_component_set_attribute( global_component,  %(Attribute)s, &%(Value)s);
			''',
    'before': '''''',
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
	'global': '''static GHashTable * %(MapName)s;''',
    'init': '''%(MapName)s = g_hash_table_new(g_str_hash, g_str_equal); GRWLock lock_%(MapName)s;''',
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
	'global': '''static GHashTable * %(MapName)s;\nGRWLock lock_%(MapName)s;''',
    'init': '''%(MapName)s = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
'horizontal_map_create_size': {
	'variables': 'MapName=activityHashTable_size',
	'global': '''static GHashTable * %(MapName)s; GRWLock lock_%(MapName)s;''',
    'init': '''%(MapName)s = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);''',
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
	'variables': 'Name=%(FUNCTION_NAME)s ComponentVariable=cv%(FUNCTION_NAME)s ActivityVariable=sioxActivity',
	'global': '''static siox_component_activity * %(ComponentVariable)s;''',
	'init': '''%(ComponentVariable)s = siox_component_register_activity( global_uid, "%(Name)s" );''',
    	'before': '''
    		assert(global_component);
	    	assert(%(ComponentVariable)s);
	    	siox_activity * %(ActivityVariable)s = siox_activity_start( global_component, %(ComponentVariable)s );''',
	'after': '''
			siox_activity_stop( %(ActivityVariable)s );''',
	'cleanup': 'siox_activity_end( %(ActivityVariable)s );',
	'final': ''
},

'guard': {
	'variables': 'Name=guard',
	'global': '''''',
	'init': '''''',
	'before': '''\tif( monitoring_namespace_deactivated() && global_component != NULL ){ ''',
	'after': '''''',
	'cleanup': '',
	'final': ''
},
'guardEnd': {
	'variables': 'Name=guard FC=%(FUNCTION_CALL)s',
	'global': '''''',
	'init': '''''',
	'before': '''''',
	'after': '''''',
	'cleanup': '\t}else{\n\t\t%(FC)s \t}',
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
	'variables': 'Attribute Value Name=G_STRFUNC Activity=sioxActivity TimeStart=NULL TimeStop=NULL',
	'global': '''''',
	'init': '''''',
	'before': '''siox_activity * %(Activity)s = siox_activity_start( global_component, %(TimeStart)s, %(Name)s );''',
	'after': '''MPI_Info * info_used;
	__real_MPI_File_get_info(fh, &info_used);
	printf("TODO: here should be a function to convert the info_used to Attribute:Value tuple in order to transfer the Hints to siox.");
	printf("TODO: before sending the Hints to siox, a checkout function should be called to filter the duplicated Hints.");
	printf("TODO: or shall we just leave the checkout to the siox activity?");
	__real_MPI_Info_free(&info_used);
	siox_activity_stop( %(Activity)s, %(TimeStop)s );''',
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
    'before': '''''',
	'after': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );',
	'cleanup': '',
	'final': ''
},

'activity_attribute_pointer': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, %(Value)s );',
	'cleanup': '',
	'final': ''
},

'activity_attribute_u32': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': ' {uint32_t u64_tmp_1 = (uint32_t) %(Value)s ; \n\tsiox_activity_set_attribute( %(Activity)s, %(Attribute)s, & u64_tmp_1 );}',
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
    'after': '''g_rw_lock_writer_lock(& lock_%(MapName)s); 
    	g_hash_table_insert( %(MapName)s, GINT_TO_POINTER(%(Key)s), siox_activity_get_ID(%(Activity)s) );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);''',
	'cleanup': '',
	'final': ''
},
'horizontal_map_put_int_ID': {
	'variables': 'Key MapName=activityHashTable_int ActivityID=sioxActivityID',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''
	if ( %(ActivityID)s != NULL ){
    	siox_activity_ID * nID = malloc(sizeof(siox_activity_ID));
    	memcpy(nID, %(ActivityID)s, sizeof(siox_activity_ID));
    	g_rw_lock_writer_lock(& lock_%(MapName)s);
    	g_hash_table_insert( %(MapName)s, GINT_TO_POINTER(%(Key)s), nID );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);
	}''',
	'cleanup': '',
	'final': ''
},

'horizontal_map_put_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''g_rw_lock_writer_lock(& lock_%(MapName)s); 
    	g_hash_table_insert( %(MapName)s, GSIZE_TO_POINTER(%(Key)s), siox_activity_get_ID(%(Activity)s) );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);''',
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
    'after': '''
    	g_rw_lock_writer_lock(& lock_%(MapName)s);
    	g_hash_table_insert( %(MapName)s, %(Key)s, siox_activity_get_ID(%(Activity)s) );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);
    	''',
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
    'after': '''
    	g_rw_lock_writer_lock(& lock_%(MapName)s);
    	g_hash_table_remove( %(MapName)s, GINT_TO_POINTER(%(Key)s) );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);
    	''',
	'cleanup': '',
	'final': ''
},
'horizontal_map_remove_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
    'after': '''
    	g_rw_lock_writer_lock(& lock_%(MapName)s);
    	g_hash_table_remove( %(MapName)s, GSIZE_TO_POINTER(%(Key)s) );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);
    	''',
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
    'after': '''
    	g_rw_lock_writer_lock(& lock_%(MapName)s);
    	g_hash_table_remove( %(MapName)s, %(Key)s );
    	g_rw_lock_writer_unlock(& lock_%(MapName)s);
    	''',
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
# The parent may be NULL in some cases
'activity_link_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''
		g_rw_lock_reader_lock(& lock_%(MapName)s); 
		siox_activity_ID * Parent = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) );
		g_rw_lock_reader_unlock(& lock_%(MapName)s);
        siox_activity_link_to_parent( %(Activity)s, Parent ); 
			  ''',
	'cleanup': '',
	'final': ''
},


'activity_link_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''
		g_rw_lock_reader_lock(& lock_%(MapName)s);
		siox_activity_ID * Parent = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GSIZE_TO_POINTER(%(Key)s) );
		g_rw_lock_reader_unlock(& lock_%(MapName)s);
        siox_activity_link_to_parent( %(Activity)s, Parent ); 
			  ''',
	'cleanup': '',
	'final': ''
},


'activity_lookup_ID_int': {
	'variables': 'Key ActivityID=Parent MapName=activityHashTable_int',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''g_rw_lock_reader_lock(& lock_%(MapName)s); 
		siox_activity_ID * %(ActivityID)s = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) ); 
		g_rw_lock_reader_unlock(& lock_%(MapName)s);''',
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
	'after': '''g_rw_lock_reader_lock(& lock_%(MapName)s); siox_activity_ID * Parent = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, %(Key)s ); g_rw_lock_reader_unlock(& lock_%(MapName)s);
    	siox_activity_link_to_parent( %(Activity)s, Parent ); 
			  ''',
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
	'global': '''''',
	'init': '''''',
    	'before': '''''',
	'after': '''if ( %(Condition)s ){
                      siox_activity_report_error( %(Activity)s, %(Error)s );
		    }''',
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
	'variables': 'RemoteCallVariable activity TargetNode TargetUID TargetIID',
	'global': '''''',
	'init': '''siox_remote_call * %(RemoteCallVariable)s;\n''',
    'before': '''%(RemoteCallVariable)s = siox_remote_call_setup( %(activity)s, %(targetNode)s, %(targetUID)s, %(TargetIID)s );''',
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
# remote_call_received
#
# Reports an attribute of a remote call just received
#
# Attribute: The attribute type of the value received
# Value: The actual value received
# Activity: The activity; defaults to sioxActivity
'remote_call_received': {
	'variables': 'RemoteCall',
	'global': '''''',
	'init': '''''',
    'before': '''siox_remote_call_start( %(RemoteCall)s );''',
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
	'after': '%(PROGRAMCODE)s',
	'cleanup': '',
	'final': ''
},

'splice_once': {
	'variables': 'PROGRAMCODE',
	'global': '%(PROGRAMCODE)s',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},


'include': {
	'variables': 'what',
	'global': '#include %(what)s',
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
        'after': 'printf("In: "); printf(__FUNCTION__); printf("\\n");\n',
        'cleanup': '',
        'final': ''
},
'rewriteCall': { # This is a special template, interpreted by the skeleton-builder !
        'variables': 'functionName arguments="" parameters=""',
        'global': '',
        'init': '',
        'before': '',
        'after': '',
        'cleanup': '',
        'final': ''
}

}

# Insert global once
globalOnce = ""

# Regexes for functions to throw away
throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

# Will be included
includes = ['<stdlib.h>', '<util/threadSafety.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>', '<C/siox.h>', '<assert.h>', '<string.h>']
