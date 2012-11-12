template = {
# register_node
# 
# Registers (and ungeristers) a new node with SIOX and sets the ontology. 
#
# SWID: The name (software id) for this node
'register_node': {
	'variables': 'SWID',
	'global': '''siox_unid global_unid;
                 char global_pid;
                 char hostname[1024];
''',
    'init': '''hostname[1023] = '\\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%%d", getpid() );
               global_unid = siox_register_node(hostname, %(SWID)s, &global_pid);
               siox_set_ontology("OntologyName");
               GHashTable *aidHashTable = g_hash_table_new(g_str_hash, g_str_equal);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'siox_unregister_node(global_unid);'
},
# datatype_register
#
# Registers a new datatype with siox
#
# RetName: Name of the variable which is used to store the pointer to the dtid
# Name: Name of the datatype
# MinStorage: The minimum storage type required to store this datatype
'datatype_register': {
	'variables': 'RetName Name MinStorage',
	'global': '''siox_dtid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_datatype( %(Name)s, %(MinStorage)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# node_attribute
#
# Reports other important attributes to SIOX
#
# DTID: Datatype-ID of the attribute
# Value: Pointer to the real value of the attribute
'node_attribute': {
	'variables': 'DTID Value',
	'global': '''''',
	'init': '''siox_node_attribute( global_unid,  %(DTID)s, %(Value)s);''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# metric_register
#
# Registers a new metric with SIOX
#
# RetName: Name of the variable to store the mid
# Name: Name of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'metric_register': {
	'variables': 'RetName Name UnitType StorageType ScopeType',
	'global': '''siox_mid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_metric( %(Name)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# activity
# 
# Starts (at the beginning) and stops (at the end) a new activity in the current function.
# Still needs to be reported!
#
# AID: Name of the varibale to store the aid
# TimeStart: Start time to be reported
# TimeStop: Stop time to be reported
# TimeEnd: End time to be reported
# Description: Short description of the activity
'activity': {
	'variables': 'AID TimeStart TimeStop TimeEnd Description',
	'global': '''''',
	'init': '''''',
    'before': '''siox_aid *%(AID)s = (siox_aid*) malloc(sizeof(siox_aid));
    			 *%(AID)s = siox_start_activity( global_unid, %(TimeStart)s, %(Description)s );''',
	'after': '''siox_stop_activity( *%(AID)s, %(TimeStop)s );
			  ''',
	'cleanup': 'siox_end_activity( *%(AID)s, %(TimeEnd)s );',
	'final': ''
},
# activity_link
# 
# Links the current activity (started with activity) to another one.
#
# AID: AID of the activity to be linked
# Key: Short description of the activity
'activity_link': {
	'variables': 'AID Key',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '''g_hash_table_insert(aidHashTable, g_strdup_printf("%i", %(Key)s), (gpointer) %(AID)s );
    			soix_aid Parent = *(siox_aid*) g_hash_table_lookup(aidHashTable, %(Key)s );
    			siox_link_activity( *%(AID)s, Parent );
			  ''',
	'cleanup': '',
	'final': ''
},
# datatype_register_as_descriptor
# 
# Starts (at the beginning) and stops (at the end) a new activity in the current function.
# Still needs to be reported!
#
# AID: Name of the varibale to store the aid
# Description: Short description of the activity
'datatype_register_as_descriptor': {
	'variables': 'DTID',
	'global': '''''',
	'init': '''''',
    'before': '''siox_register_datatype_as_descriptor( %(DTID)s );''',
	'after': '''''',
	'cleanup': '',
	'final': ''
},
# report
#
# Reports an activity
#
# AID: The aid of the activity to be reported
# MID: Metric-ID of the reported value
# Value: Pointer to the value to be reported
'report': {
	'variables': 'AID MID Value',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report( %(AID)s, %(MID)s, (void *) %(Value)s );',
	'cleanup': '',
	'final': ''
},
# report
#
# Reports an error
#
# AID: The aid of the activity to be reported
# Error: The error to be reported
'report_error': {
	'variables': 'AID ERROR',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report_error( %(AID)s, %(ERROR)s );',
	'cleanup': '',
	'final': ''
},
# remote_call_start
#
# Initiates a new remote call
#
# RCID: The name of the varibale to store the RCID
# AID: The corresponding AID
# HWID: The hardware id of the addressed machine
# SWID: The software id of the addressed machine
# IID: The instance id of the addressed machine
'remote_call_start': {
	'variables': 'RCID AID HWID SWID IID',
	'global': '''siox_rcid %(RCID)s;\n''',
	'init': '''''',
    'before': '''%(RCID)s = siox_describe_remote_call_start( %(AID)s, %(HWID)s, %(SWID)s, %(IID)s );''',
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
    'before': '''siox_remote_call_attribute( %(RCID)s, %(DTID)s, (void *) %(Value)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# remote_call_receive
#
# Receives a remote call
#
# AID: The corresponding AID
# DTID: The datatype id of the value
# Value: The received value
'remote_call_receive': {
	'variables': 'AID DTID Value',
	'global': '''siox_rcid %(RCID)s;\n''',
	'init': '''''',
    'before': '''siox_remote_call_receive( %(RCID)s, %(DTID)s, (void *) %(Value)s );''',
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
# splice_before
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
includes = ['<siox-ll.h>', '<stdarg.h>']
