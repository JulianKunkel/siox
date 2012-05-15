template = {
'ontology': {
	'variables': 'VARNAME VARTYPE',
	'global': 'siox_dtid dtid%(VARNAME)s;',
	'init': 'dtid%(VARNAME)s = siox_register_datatype( %(VARNAME)s, %(VARTYPE)s );',
	'before': '',
	'after': '',
	'final': ''
},
'register_node': {
	'variables': 'HWID SWID',
	'global': '''siox_unid global_unid; %(HWID)s
                 char global_pid;''',
    'init': '''sprintf( global_pid, "%%d", getpid() );
               global_unid = siox_register_node("%(HWID)s", "%(SWID)s", global_pid);''',
	'before': '',
	'after': '',
	'final': 'siox_unregister_node(global_unid);'
},
'register_descriptor': {
	'variables': 'Typ1 Typ2 DmidName',
	'global': '''siox_dmid %(DmidName)s;''',
	'init': '''%(DmidName)s = siox_register_descriptor_map( global_unid, %(Typ1)s, %(Typ2)s );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'register_edge': {
	'variables': 'Edge',
	'global': '''''',
	'init': '''siox_register_edge( global_unid, "%(Edge)s" );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'activity': {
	'variables': 'Name Description',
	'global': '''siox_aid %(Name)s;''',
	'init': '''''',
    'before': '''%(Name)s = siox_start_activity( global_unid, "%(Name)s" );''',
	'after': 'siox_stop_activity( "%(Name)s" );',
	'final': ''
},
'activity_start': {
	'variables': 'Name Description',
	'global': '''siox_aid %(Name)s;''',
	'init': '''''',
    'before': '''%(Name)s = siox_start_activity( global_unid, "%(Name)s" );''',
	'after': '',
	'final': ''
},
'activity_stop': {
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_stop_activity( "%(Name)s" );',
	'final': ''
},
'activity_end': {
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_end_activity( "%(Name)s" );',
	'final': ''
},
'activity_report': { #TODO
	'variables': '',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': '',
	'final': ''
},
'create_descriptor': {
	'variables': 'DescriptorName Type',
	'global': '''''',
	'init': '''siox_create_descriptor( global_unid, "%(DescriptorName)s", "%(Type)s");''',
    'before': '''''',
	'after': '',
	'final': 'siox_release_descriptor( global_unid, "%(DescriptorName)s", "%(Type)s");'
},
'send_descriptor': {
	'variables': 'TargetSWID DescriptorName Type',
	'global': '''''',
	'init': '''''',
    'before': '''siox_send_descriptor( global_unid, "%(TargetSWID)s", "%(DescriptorName)s", "%(Type)s");''',
	'after': '',
	'final': ''
},
'map_descriptor': {
	'variables': 'Descriptor From To',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_map_descriptor( global_unid, %(Descriptor)s, %(From)s, %(To)s);',
	'final': ''
},
'receive_descriptor': {
	'variables': 'Descriptor From To',
	'global': '''''',
	'init': '''''',
    'before': '''siox_receive_descriptor( global_unid, %(Descriptor)s, %(From)s, %(To)s);''',
	'after': '',
	'final': ''
},
'splice': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '%(PROGRAMMCODE)s',
	'after': '',
	'final': ''
}
}

throwaway = ["^\s*extern\s*'"]