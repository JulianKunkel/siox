template = {
'register_node': { # angepasst
	'variables': 'HWID SWID OntName',
	'global': '''siox_unid global_unid;
                 char global_pid;''',
    'init': '''sprintf( global_pid, "%%d", getpid() );
               global_unid = siox_register_node("%(HWID)s", "%(SWID)s", global_pid);
               siox_set_ontology("%(OntName)s");''',
	'before': '',
	'after': '',
	'final': 'siox_unregister_node(global_unid);'
},
'register_descriptor': { # angepasst
	'variables': 'Typ1 Typ2 DmidName',
	'global': '''siox_dmid %(DmidName)s;''',
	'init': '''%(DmidName)s = siox_register_descriptor_map( global_unid, %(Typ1)s, %(Typ2)s );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'register_edge': { # angepasst
	'variables': 'Edge',
	'global': '''''',
	'init': '''siox_register_edge( global_unid, "%(Edge)s" );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'register_datatype': { # angepasst
	'variables': 'Name MinStorage RetName',
	'global': '''siox_dtid %(RetName)s;''',
	'init': '''%(RetName)s = siox_register_datatype( "%(Name)s", "%(MinStorage)s" );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'register_attribute': { # angepasst
	'variables': 'Key ValueType Value',
	'global': '''''',
	'init': '''siox_register_attribute( global_unid, "%(Key)s", "%(ValueType)s", "%(Value)s" );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'register_metric': { # angepasst
	'variables': 'Name Description UnitType StorageType ScopeType',
	'global': '''''',
	'init': '''siox_register_metric( "%(Name)s", "%(Description)s", "%(UnitType)s", "%(StorageType)s", "%(ScopeType)s" );''',
    'before': '''''',
	'after': '',
	'final': ''
},
'activity': { # angepasst
	'variables': 'Name Description',
	'global': '''siox_aid %(Name)s;''',
	'init': '''''',
    'before': '''%(Name)s = siox_start_activity( global_unid, "%(Description)s" );''',
	'after': 'siox_stop_activity( "%(Name)s" );',
	'final': ''
},
'activity_start': { # angepasst
	'variables': 'Name Description',
	'global': '''siox_aid %(Name)s;''',
	'init': '''''',
    'before': '''%(Name)s = siox_start_activity( global_unid, "%(Description)s" );''',
	'after': '',
	'final': ''
},
'activity_stop': { # angepasst
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_stop_activity( "%(Name)s" );',
	'final': ''
},
'activity_end': { # angepasst
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_end_activity( "%(Name)s" );',
	'final': ''
},
'activity_report': { # angepasst
	'variables': 'ActivityId DescriptorId DescriptorName MapId ValueType Value Details',
	'global': '''''',
	'init': '''''',
    'before': '''siox_report_activity(%(ActivityId)s, %(DescriptorId)s, %(DescriptorName)s, %(MapId)s, %(ValueType)s, %(Value)s, %(Details)s);''',
	'after': '',
	'final': ''
},
'create_descriptor': { # angepasst
	'variables': 'Type DescriptorName',
	'global': '''''',
	'init': '''siox_create_descriptor( global_unid, "%(Type)s", "%(DescriptorName)s");''',
    'before': '''''',
	'after': '',
	'final': 'siox_release_descriptor( global_unid, "%(Type)s", "%(DescriptorName)s");'
},
'send_descriptor': { # angepasst
	'variables': 'TargetSWID Type DescriptorName',
	'global': '''''',
	'init': '''''',
    'before': '''siox_send_descriptor( global_unid, "%(TargetSWID)s", "%(Type)s", "%(DescriptorName)s" );''',
	'after': '',
	'final': ''
},
'map_descriptor': { # angepasst
	'variables': 'Descriptor From To',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_map_descriptor( global_unid, %(Descriptor)s, %(From)s, %(To)s);',
	'final': ''
},
'receive_descriptor': { # angepasst
	'variables': 'Type Name',
	'global': '''''',
	'init': '''''',
    'before': '''siox_receive_descriptor( global_unid, %(Type)s, %(Name)s );''',
	'after': '',
	'final': ''
},
'splice_before': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '%(PROGRAMMCODE)s',
	'after': '',
	'final': ''
},
'splice_after': {
	'variables': 'PROGRAMMCODE',
	'global': '',
	'init': '',
	'before': '',
	'after': '%(PROGRAMMCODE)s',
	'final': ''
}
}

throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

includes = ["<stdio.h>", "<stdlib.h>", "<hdf5.h>", "\"siox-ll.h\""]

dlsymLibPath = "\"/usr/local/\""

dllib = "\"libhdf5.dylib\""