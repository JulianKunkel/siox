template = {
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
'register_descriptor_map': {
	'variables': 'DmidName Typ1 Typ2',
	'global': '''siox_dmid %(DmidName)s;''',
	'init': '''%(DmidName)s = siox_register_descriptor_map( global_unid, %(Typ1)s, %(Typ2)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_edge': {
	'variables': 'Edge',
	'global': '''''',
	'init': '''siox_register_edge( global_unid, %(Edge)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_datatype': {
	'variables': 'RetName Name MinStorage',
	'global': '''siox_dtid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_datatype( %(Name)s, %(MinStorage)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_attribute': {
	'variables': 'Key ValueType Value',
	'global': '''''',
	'init': '''siox_register_attribute( global_unid, %(Key)s, %(ValueType)s, %(Value)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_metric': {
	'variables': 'RetName Name Description UnitType StorageType ScopeType',
	'global': '''siox_mid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_metric( %(Name)s, %(Description)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
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
'activity_report': {
	'variables': 'AID Type Descriptor MID value details',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report_activity(%(AID)s, %(Type)s, %(Descriptor)s, %(MID)s, %(value)s, %(details)s);',
	'cleanup': '',
	'final': ''
},
'create_descriptor': {
	'variables': 'AID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_create_descriptor( %(AID)s, %(Type)s, %(Descriptor)s);',
	'cleanup': '',
	'final': 'siox_release_descriptor( global_unid, %(Type)s, %(DescriptorName)s);'
},
'send_descriptor': {
	'variables': 'AID TargetSWID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''siox_send_descriptor( %(AID)s, %(TargetSWID)s, %(Type)s, %(Descriptor)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'map_descriptor': {
	'variables': 'AID Type OldDescriptor NewDescriptor',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_map_descriptor( %(AID)s, %(Type)s, %(OldDescriptor)s, %(NewDescriptor)s);',
	'cleanup': '',
	'final': ''
},
'receive_descriptor': {
	'variables': 'AID Type Descriptor',
	'global': '''''',
	'init': '''''',
    'before': '''siox_receive_descriptor( %(AID)s, %(Type)s, %(Descriptor)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'splice_before': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '%(PROGRAMMCODE)s',
	'after': '',
	'cleanup': '',
	'final': ''
},
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

forEachBefore = ""

forEachAfter = ""

throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

includes = ['<siox-ll.h>', '<stdargs.h>']

# Old stuff
# 'activity_start': {
# 	'variables': 'Name Description',
# 	'global': '''siox_aid %(Name)s;''',
# 	'init': '''''',
#     'before': '''%(Name)s = siox_start_activity( global_unid, %(Description)s );''',
# 	'after': '',
# 	'cleanup': '',
# 	'final': ''
# },
# 'activity_stop': {
# 	'variables': 'Name',
# 	'global': '''''',
# 	'init': '''''',
#     'before': '''''',
# 	'after': 'siox_stop_activity( %(Name)s );',
# 	'cleanup': '',
# 	'final': ''
# },
# 'activity_end': {
# 	'variables': 'Name',
# 	'global': '''''',
# 	'init': '''''',
#     'before': '''''',
# 	'after': 'siox_end_activity( %(Name)s );',
# 	'cleanup': '',
# 	'final': ''
# },
# 'release_descriptor': {
# 	'variables': 'AID Type Descriptor',
# 	'global': '''''',
# 	'init': '''''',
#     'before': '''''',
# 	'after': '',
# 	'cleanup': 'siox_release_descriptor( %(AID)s, %(Type)s, %(Descriptor)s);',
# 	'final': ''
# },
