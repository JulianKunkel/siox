template = {
# component
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
# InterfaceName = Name of interface
# ImplementationIdentifier ?
# InstanceName ?
# ComponentVariable ? ?
# SpliceCode ? ? ?

# Special tag, annotates all functions, here we measure the overhead of SIOX itself (profile)
# Here we capture the overhead of all fkt calls.
# NOTE THIS IS NOT REALLY THREAD SAFE!!!
'ALL_FUNCTIONS':{
   'variables': 'fname=%(FUNCTION_NAME)s',
   'global': '''static siox_timestamp t_overhead_%(fname)s = 0;
static siox_timestamp t_fkt_%(fname)s = 0;
static int calls_%(fname)s = 0;''',
	'before' : '''
			siox_timestamp t_tmp = 0;
			siox_timestamp t_fkt_start = 0;
			siox_timestamp t_start = siox_gettime();
			siox_timestamp t_overhead = 0;''',
	'beforeLast' : '''
		//t_tmp = siox_gettime();
		t_overhead = t_tmp - t_start;
		t_fkt_start = t_tmp;
		''',
	'after' : '''
		//t_tmp = siox_gettime();
		t_start = t_tmp;
		t_fkt_%(fname)s += t_tmp - t_fkt_start;
		''',
	'cleanupLast' : '''
		calls_%(fname)s++;
		t_overhead += siox_gettime() - t_start;
		t_overhead_%(fname)s += t_overhead;
		''',
	'finalOnce' : '''
		FILE * overheadFD = NULL;
		siox_timestamp overheadTotal = 0;
		if ( getenv("SIOX_WRITE_OVERHEAD_FILE") ){
			char overheadFile[40];
			sprintf(overheadFile, "%%s_%%lld.txt", getenv("SIOX_WRITE_OVERHEAD_FILE"), (long long int) getpid());
			overheadFD = fopen(overheadFile, "a");
			fprintf(overheadFD, "function\\t\\tcalls\\t\\ttime\\t\\t\\torig\\t\\toverhead\\trel. overhead\\n");
		}
		''',
	'finalOnceLast' : '''
		if (overheadFD){
			fprintf(overheadFD, "total overhead: %%fs\\n", siox_time_in_s(overheadTotal));
			fclose(overheadFD);
		}
		''',
	'final' : '''
		if (overheadFD && calls_%(fname)s > 0){
			fprintf(overheadFD, "%(fname)s()\\t"
				"%%d\\t"
				"%%fms\\t"
				"%%fms/call\\t\\t"
				"%%fms/call\\t\\t"
				"%%.2f%%%%/call\\n",
				calls_%(fname)s,
				siox_time_in_s(t_fkt_%(fname)s + t_overhead_%(fname)s) * 1000,
				siox_time_in_s(t_fkt_%(fname)s) / calls_%(fname)s * 1000,
				siox_time_in_s(t_overhead_%(fname)s) / calls_%(fname)s * 1000,
				100.0 * t_overhead_%(fname)s / t_fkt_%(fname)s
				);
				overheadTotal += t_overhead_%(fname)s;
		}
	'''
},
'component': {
	'variables': 'InterfaceName ImplementationIdentifier InstanceName="" ComponentVariable=global SpliceCode=',
	'global': '''
static siox_component * %(ComponentVariable)s_component = NULL;
static siox_unique_interface * %(ComponentVariable)s_uid = NULL;
static int %(ComponentVariable)s_layer_initialized = FALSE;
		''',
    'init': '''
		if ( siox_is_monitoring_permanently_disabled() || %(ComponentVariable)s_component ){
				return;
		}
      %(SpliceCode)s
		%(ComponentVariable)s_uid = siox_system_information_lookup_interface_id("%(InterfaceName)s", "%(ImplementationIdentifier)s");

		// avoid double instrumentation with DLSYM and LD_PRELOAD
		if ( siox_component_is_registered( %(ComponentVariable)s_uid ) ){
			fprintf(stderr, "WARNING: layer '%%s/%%s' is already instrumented, do not use LD_PRELOAD again! Most likely the application breaks.\\n", "%(InterfaceName)s", "%(ImplementationIdentifier)s");
			return;
		}
		%(ComponentVariable)s_component = siox_component_register(%(ComponentVariable)s_uid, "%(InstanceName)s");
		siox_register_initialization_signal(sioxInit);
      siox_register_termination_signal(sioxFinal);

		''',
        'initLast': '%(ComponentVariable)s_layer_initialized = TRUE;',
	'final': '''
		if (%(ComponentVariable)s_layer_initialized) { siox_component_unregister(%(ComponentVariable)s_component); %(ComponentVariable)s_component = NULL; %(ComponentVariable)s_layer_initialized = FALSE; }'''
},
'autoInitializeLibrary':{
	'global' : """
				static void sioxFinal() __attribute__((destructor));
static void sioxInit() __attribute__((constructor));
            """
},
'createInitializerForLibrary':{
	'global' : """
				static void sioxFinal();
            static void sioxInit();
            """
},
'callLibraryFinalize':{
	'cleanup' : 'sioxFinal();'
},
'callLibraryFinalizeBefore':{
	'before' : 'sioxFinal();'
},
'callLibraryInitialize':{
	'before' : 'sioxInit();'
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
	'global': '''static siox_attribute * %(AttributeVariable)s;''',
	'init': '''%(AttributeVariable)s = siox_ontology_register_attribute( "%(Domain)s", "%(Name)s", %(StorageType)s ); assert(%(AttributeVariable)s != NULL);''',
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
				{
				%(SpliceCode)s
				assert( %(Attribute)s != NULL );
				siox_component_set_attribute( global_component,  %(Attribute)s, %(Value)s);
				}
			''',
    'before': '''''',
	'cleanup': '',
	'final': ''
},
'component_attribute': {
	'variables': 'Attribute Value SpliceCode=',
	'global': '''''',
	'after': '''
				{
				%(SpliceCode)s
				assert( %(Attribute)s != NULL );
				siox_component_set_attribute( global_component,  %(Attribute)s, &%(Value)s);
				}
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
	'variables': 'Name=%(FUNCTION_NAME)s ComponentActivity=cv%(FUNCTION_NAME)s ComponentVariable=global ActivityVariable=sioxActivity',
	'global': '''static siox_component_activity * %(ComponentActivity)s;''',
	'init': '''%(ComponentActivity)s = siox_component_register_activity( %(ComponentVariable)s_uid, "%(Name)s" );''',
    	'before': '''
	    	assert(%(ComponentVariable)s_component);
	    	assert(%(ComponentActivity)s);
	    	siox_activity * %(ActivityVariable)s = siox_activity_begin( %(ComponentVariable)s_component, %(ComponentActivity)s );''',
	   'beforeLast': '''t_tmp = siox_activity_start(%(ActivityVariable)s);''',
	'afterFirst': '''
			t_tmp = siox_activity_stop( %(ActivityVariable)s );
            siox_activity_end_keep( %(ActivityVariable)s );''',
	'cleanup': 'siox_activity_kept_delete( %(ActivityVariable)s );',
	'final': ''
},
# This template allows to inject an activity into a component in which it belongs to.
'activityComponentSwitcher2BasedOnParent': {
	'variables': 'Name=%(FUNCTION_NAME)s Key=unknown ComponentVariable1=global ComponentVariable2=global MapName1=activityHashTable_int MapName2=activityHashTable_int2 ActivityVariable=sioxActivity ActivityParentVar=parent',
	'global': '''
				static siox_component_activity * %(Name)s_%(ComponentVariable1)s;
				static siox_component_activity * %(Name)s_%(ComponentVariable2)s;
					''',
	'init': '''
	%(Name)s_%(ComponentVariable1)s = siox_component_register_activity( %(ComponentVariable1)s_uid, "%(Name)s" );
	%(Name)s_%(ComponentVariable2)s = siox_component_register_activity( %(ComponentVariable2)s_uid, "%(Name)s" );''',

    	'before': '''
    		g_rw_lock_reader_lock(& lock_%(MapName1)s);
			siox_activity_ID * %(ActivityParentVar)s = (siox_activity_ID*) g_hash_table_lookup( %(MapName1)s, GINT_TO_POINTER(%(Key)s) );
			g_rw_lock_reader_unlock(& lock_%(MapName1)s);
			// now decide to which component the activity parent belongs to
			// we expect it is likely to belong to the first component
			siox_activity * %(ActivityVariable)s = NULL;

			if ( %(ActivityParentVar)s == NULL ){
				// check if it belongs to the other component
				g_rw_lock_reader_lock(& lock_%(MapName2)s);
			   %(ActivityParentVar)s = (siox_activity_ID*) g_hash_table_lookup( %(MapName2)s, GINT_TO_POINTER(%(Key)s) );
				g_rw_lock_reader_unlock(& lock_%(MapName2)s);

				if( %(ActivityParentVar)s != NULL ){
					%(ActivityVariable)s = siox_activity_begin( %(ComponentVariable2)s_component, %(Name)s_%(ComponentVariable2)s );
				}else{
				   // unknown so we keep the first component
					%(ActivityVariable)s = siox_activity_begin( %(ComponentVariable1)s_component, %(Name)s_%(ComponentVariable1)s );
				}
			}else{
				%(ActivityVariable)s = siox_activity_begin( %(ComponentVariable1)s_component, %(Name)s_%(ComponentVariable1)s );
			}


	    	siox_activity_link_to_parent( %(ActivityVariable)s, %(ActivityParentVar)s );
	    	''',
	   'beforeLast': '''t_tmp = siox_activity_start(%(ActivityVariable)s);''',
	'afterFirst': '''t_tmp = siox_activity_stop( %(ActivityVariable)s );''',
	'cleanup': 'siox_activity_end( %(ActivityVariable)s );',
},
'guard': {
	'variables': 'Name=guard FC=%(FUNCTION_CALL)s ComponentVariable=global',
	'global': '''''',
	'init': '''''',
	'before': '''\tif( siox_monitoring_namespace_deactivated() && %(ComponentVariable)s_layer_initialized && siox_is_monitoring_enabled() ){ ''',
	'after': '''''',
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
	'before': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );',
},
'activity_attribute_pointer': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, %(Value)s );',
},
'activity_attribute_str': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, %(Value)s );',
},
'activity_attribute_u32': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'before': ' {uint32_t u64_tmp_1 = (uint32_t) %(Value)s ; \n\tsiox_activity_set_attribute( %(Activity)s, %(Attribute)s, & u64_tmp_1 );}',
},
# PLAYBACK
# When using playback plugins the late attributes usually refers to values
# the system/library calls. Therefor we replace all set_attributes with get,
# since the playback plugin should attach the return values.
'activity_attribute_late': {
	'variables': 'Attribute Value Activity=sioxActivity',
    'after': """
//siox_activity_set_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );
siox_activity_get_attribute( %(Activity)s, %(Attribute)s, &%(Value)s );
    """,
},
'activity_attribute_late_pointer': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'after': 'siox_activity_set_attribute( %(Activity)s, %(Attribute)s, %(Value)s );',
},
'activity_attribute_late_u32': {
	'variables': 'Attribute Value Activity=sioxActivity',
	'after': ' {uint32_t u64_tmp_1 = (uint32_t) %(Value)s ; \n\tsiox_activity_set_attribute( %(Activity)s, %(Attribute)s, & u64_tmp_1 );}',
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
# Tie an attribute serving as a descriptor to an activity.
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
# Clear an attribute serving as a descriptor.
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
# Clear an attribute serving as a descriptor.
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
# a descriptor represented as either an int or a long int.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in int or long int form
# Activity: Activity to be linked; defaults to sioxActivity
# The parent may be NULL in some cases
'activity_link_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '''
		{
		g_rw_lock_reader_lock(& lock_%(MapName)s);
		siox_activity_ID * Parent = (siox_activity_ID*) g_hash_table_lookup( %(MapName)s, GINT_TO_POINTER(%(Key)s) );
		g_rw_lock_reader_unlock(& lock_%(MapName)s);
        siox_activity_link_to_parent( %(Activity)s, Parent );
      }
			  ''',
	'cleanup': '',
	'final': ''
},
'activity_link_size': {
	'variables': 'Key MapName=activityHashTable_size Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
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
	'before': '''siox_activity_link_to_parent( %(Activity)s, %(Parent)s ); ''',
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
                      siox_activity_end(%(Activity)s);
                      return ret;
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

'splice_once_end': {
	'variables': 'PROGRAMCODE',
	'global_end': '%(PROGRAMCODE)s',
	'init': '',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},

'include': {
	'variables': 'what',
	'global': '#include "%(what)s"',
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
        'variables': 'functionName arguments= parameters=',
        'global': '',
        'init': '',
        'before': '',
        'after': '',
        'cleanup': '',
        'final': ''
}
,
'supressFunctionCall': { # This is a special template, interpreted by the skeleton-builder !
        'variables': '',
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
"includes" : ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>', '<C/siox.h>', '<assert.h>', '<string.h>', '<unistd.h>']
}
