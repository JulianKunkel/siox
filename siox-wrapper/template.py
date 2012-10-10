template = {
# interface_name 
# 
# Registers (and ungeristers) a new node with SIOX and sets the ontology. 
#
# SWID: The name for this node
'interface_name': {
	'variables': 'SWID',
	'global': '''siox_unid global_unid;
                 char global_pid;
                 char hostname[1024];
''',
    'init': '''hostname[1023] = '\\0';
			   gethostname(hostname, 1023);
               sprintf( &global_pid, "%%d", getpid() );
               global_unid = siox_register_node(hostname, %(SWID)s, &global_pid);
               siox_set_ontology("OntologyName");''',
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
# attribute_register
#
# Reports other important attributes to SIOX
#
# DTID: Datatype-ID of the attribute
# Value: Pointer to the real value of the attribute
'attribute_register': {
	'variables': 'DTID Value',
	'global': '''''',
	'init': '''siox_register_attribute( global_unid,  %(DTID)s, %(Value)s);''',
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
# Description: Short description of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'metric_register': {
	'variables': 'RetName Name Description UnitType StorageType ScopeType',
	'global': '''siox_mid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_metric( %(Name)s, %(Description)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
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
# Description: Short description of the activity
'activity': {
	'variables': 'AID Description',
	'global': '''''',
	'init': '''''',
    'before': '''siox_aid %(AID)s = siox_start_activity( global_unid, %(Description)s );''',
	'after': '''siox_stop_activity( %(AID)s );
			  ''',
	'cleanup': 'siox_end_activity( %(AID)s );',
	'final': ''
},
# acivity_report
#
# Reports an activity
#
# AID: The aid of the activity to be reported
# Type: Type of the descriptor the activity is dedicated to
# Descriptor: Pointer to the descriptor
# MID: Metric-ID of the reported value
# Value: Pointer to the value to be reported
# Details: Any explanatory notes
'activity_report': {
	'variables': 'AID DescriptorType Descriptor MID value details',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report_activity(%(AID)s, %(DescriptorType)s, &%(Descriptor)s, %(MID)s, &%(value)s, %(details)s);',
	'cleanup': '',
	'final': ''
},
# descriptor_map_register
#
# Tells SIOX what types of descriptors can be mapped by this node.
#
# DmidName: Name of the variable which is used to store the pointer to the dmid
# Typ1: The type of the origin
# Typ2: The type of the target
'descriptor_map_register': {
	'variables': 'DmidName Typ1 Typ2',
	'global': '''siox_dmid %(DmidName)s;''',
	'init': '''%(DmidName)s = siox_register_descriptor_map( global_unid, %(Typ1)s, %(Typ2)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# descriptor_create
#
# Creates a new descriptor
#
# AID: The activity-ID associated
# Type: The dtid of the descriptor
# Descriptor: The descriptor
'descriptor_create': {
	'variables': 'AID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_create_descriptor( %(AID)s, %(Type)s, &%(Descriptor)s);',
	'cleanup': '',
	'final': 'siox_release_descriptor( global_unid, %(Type)s, &%(DescriptorName)s);'
},
# descriptor_send
#
# Sends a descriptor to another node
#
# AID: The acitivity associated
# TargetSWID: The receiving node
# Type: The type (dtid) of the descriptor associated
# Descriptor: The descriptor associated
'descriptor_send': {
	'variables': 'AID TargetSWID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''siox_send_descriptor( %(AID)s, %(TargetSWID)s, %(Type)s, &%(Descriptor)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# descriptor_map
#
# Maps a descriptor
#
# AID: The activity-id associated
# Type: Type of the descriptor
# OldDescriptor: The old descriptor (to be mapped)
# NewDescriptor: The new descriptor (emerged from the old descriptor)
'descriptor_map': {
	'variables': 'AID Type OldDescriptor NewDescriptor',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_map_descriptor( %(AID)s, %(Type)s, &%(OldDescriptor)s, &%(NewDescriptor)s);',
	'cleanup': '',
	'final': ''
},
# descriptor_receive
#
# Receives a descriptor
#
# AID: The id of the activity associated
# Type: The dtid of the descriptor
# Descriptor: The descriptor
'descriptor_receive': {
	'variables': 'AID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''siox_receive_descriptor( %(AID)s, %(Type)s, &%(Descriptor)s );''',
	'after': '',
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
includes = ['<siox-ll.h>', '<stdargs.h>']