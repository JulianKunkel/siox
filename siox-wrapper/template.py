template = {
# component_register
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
#
# SWID: The name (software id) for this component
'component_register': {
	'variables': 'SWID',
	'global': '''siox_component global_component;
				 siox_ontology global_ontology;
                 char global_pid;
                 char hostname[1024];
''',
    'init': '''hostname[1023] = '\\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%%d", getpid() );
               global_component = siox_component_register(hostname, %(SWID)s, &global_pid);
               global_ontology = siox_ontology_connect();
               GHashTable *activityHashTable = g_hash_table_new(g_str_hash, g_str_equal);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'siox_component_unregister(global_component);'
},
# attribute_register
#
# Registers a new attribute type with siox, so that later calls
# can set values for this type of attribute on certain entities.
#
# RetName: Name of the variable which is used to store the
#		   pointer to the attribute type
# Name: Name of the attribute
# MinStorage: The minimum storage type required to store this
#			  attribute
'attribute_register': {
	'variables': 'RetName Name MinStorage',
	'global': '''siox_attribute %(RetName)s;''',
	'init': '''%(RetName)s = siox_attribute_register( %(Name)s, %(MinStorage)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# component_register_descriptor
#
# Mark an attribute type to serve as a descriptor for this component.
#
# Attribute: Name of the attribute type to mark as a descriptor
'component_register_descriptor': {
	'variables': 'Attribute',
	'global': '''siox_component_register_descriptor( global_component, %(Attribute)s );''',
	'init': '''''',
    'before': '''''',
	'after': '''''',
	'cleanup': '',
	'final': ''
},
# component_set_attribute
#
# Reports the value of one of the component's attributes to SIOX
#
# Attribute: Attribute type to set; must have been registered via
#			 siox_attribute_register().
# Value: Pointer to the real value of the attribute
'component_set_attribute': {
	'variables': 'Attribute Value',
	'global': '''''',
	'init': '''siox_component_set_attribute( global_component,  %(Attribute)s, %(Value)s);''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# metric_register
#
# Registers a new metric with SIOX.
# Metrics are held in an ontology, hence the name.
#
# RetName: Name of the variable to store the metric type in
# Name: Name of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'metric_register': {
	'variables': 'RetName Name UnitType StorageType ScopeType',
	'global': '''siox_metric %(RetName)s;''',
	'init': '''%(RetName)s = siox_ontology_register_metric( global_ontology, %(Name)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# activity
#
# Starts (at the beginning) and stops (at the end) a new
# activity in the current function.
# Still needs to be reported!
#
# Activity: Name of the variable to store the activity in
# TimeStart: Start time to be reported
# TimeStop: Stop time to be reported
# TimeEnd: End time to be reported
# Description: Short description of the activity
'activity': {
	'variables': 'Activity TimeStart TimeStop Description',
	'global': '''''',
	'init': '''''',
    'before': '''siox_activity *%(Activity)s = (siox_activity*) malloc(sizeof(siox_activity));
    			 *%(Activity)s = siox_activity_start( global_component, %(TimeStart)s, %(Description)s );''',
	'after': '''siox_stop_activity( *%(Activity)s, %(TimeStop)s );
			  ''',
	'cleanup': 'siox_end_activity( *%(Activity)s;',
	'final': ''
},
# activity_link
#
# Links the current activity (started with activity) to another one.
#
# Activity: Activity to be linked
# Key: Short description of the activity
'activity_link': {
	'variables': 'Activity Key',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''g_hash_table_insert(activityHashTable, g_strdup_printf("%i", %(Key)s), (gpointer) %(Activity)s );
    			soix_Activity Parent = *(siox_Activity*) g_hash_table_lookup(activityHashTable, %(Key)s );
    			siox_link_activity( *%(Activity)s, Parent );
			  ''',
	'cleanup': '',
	'final': ''
},
# report
#
# Reports an activity
#
# Component: The Component
# Metric: Metric of the reported value
# Value: Pointer to the value to be reported
'report': {
	'variables': 'Component Metric Value',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report( %(Component)s, %(Metric)s, (void *) &%(Value)s );',
	'cleanup': '',
	'final': ''
},
# report_error
#
# Reports an error
#
# Activity: The Activity to be reported
# Error: The error to be reported
'report_error': {
	'variables': 'Activity ERROR',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report_error( %(Activity)s, %(ERROR)s );',
	'cleanup': '',
	'final': ''
},
# remote_call_start
#
# Initiates a new remote call
#
# RCID: The name of the varibale to store the RCID
# Activity: The corresponding Activity
# HWID: The hardware id of the addressed machine
# SWID: The software id of the addressed machine
# IID: The instance id of the addressed machine
'remote_call_start': {
	'variables': 'RCID Activity HWID SWID IID',
	'global': '''siox_rcid %(RCID)s;\n''',
	'init': '''''',
    'before': '''%(RCID)s = siox_describe_remote_call_start( %(Activity)s, %(HWID)s, %(SWID)s, %(IID)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_attribute
#
# Send an attribute over a remote call
#
# RCID: The id of the corresponding remote call
# DTID: The datatype id of the value
# Value: Value to be send
'remote_call_attribute': {
	'variables': 'RCID DTID Value',
	'global': '''''',
	'init': '''''',
    'before': '''siox_remote_call_attribute( %(RCID)s, %(DTID)s, (void *) &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_receive
#
# Receives a remote call
#
# Activity: The corresponding Activity
# DTID: The datatype id of the value
# Value: The received value
'remote_call_receive': {
	'variables': 'Activity DTID Value',
	'global': '''siox_rcid %(RCID)s;\n''',
	'init': '''''',
    'before': '''siox_remote_call_receive( %(RCID)s, %(DTID)s, (void *) &%(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_end
#
# Closes a remote call
#
# RCID: The RCID of the remote call to be closed
'remote_call_end': {
	'variables': 'RCID',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_describe_remote_call_end( %(RCID)s );',
	'cleanup': '',
	'final': ''
},
# splice_before
#
# Used to insert custom code before the call
#
# PROGRAMMCODE: The code to insert
'splice_before': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '%(PROGRAMMCODE)s',
	'after': '',
	'cleanup': '',
	'final': ''
},
# splice_after
#
# Used to insert custom code after the call
#
# PROGRAMMCODE: The code to insert
'splice_after': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '%(PROGRAMMCODE)s',
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
includes = ['<stdlib.h>', '<siox-ll.h>', '<stdarg.h>', '<glib.h>']
