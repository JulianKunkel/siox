template = {
'interfaceName': {
	'variables': 'SWID',
	'global': '''siox_unid global_unid;
                 char global_pid;''',
    'init': '''sprintf( &global_pid, "%%d", getpid() );
               global_unid = siox_register_node("TODO", "%(SWID)s", &global_pid);
               siox_set_ontology("OntologyName");''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'siox_unregister_node(global_unid);'
},
'register_descriptor': {
	'variables': 'Typ1 Typ2 DmidName',
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
	'init': '''siox_register_edge( global_unid, "%(Edge)s" );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_datatype': { 
	'variables': 'Name MinStorage RetName',
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
	'init': '''siox_register_attribute( global_unid, "%(Key)s", "%(ValueType)s", "%(Value)s" );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'register_metric': {
	'variables': 'Name Description UnitType StorageType ScopeType RetName',
	'global': '''siox_mid %(RetName)s''',
	'init': '''%(RetName)s = siox_register_metric( %(Name)s, %(Description)s, %(UnitType)s, %(StorageType)s, %(ScopeType)s );''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'activity': { 
	'variables': 'Description',
	'global': '''''',
	'init': '''''',
    'before': '''siox_aid local_aid = siox_start_activity( global_unid, "%(Description)s" );''',
	'after': '''siox_stop_activity( local_aid );
			  siox_end_activity( local_aid );
			  ''',
	'cleanup': '',
	'final': ''
},
'activity_start': {
	'variables': 'Name Description',
	'global': '''siox_aid %(Name)s;''',
	'init': '''''',
    'before': '''%(Name)s = siox_start_activity( global_unid, "%(Description)s" );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'activity_stop': { 
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_stop_activity( "%(Name)s" );',
	'cleanup': '',
	'final': ''
},
'activity_end': {
	'variables': 'Name',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_end_activity( "%(Name)s" );',
	'cleanup': '',
	'final': ''
},
'activity_report': {
	'variables': 'ActivityId DescriptorId DescriptorName MetricId Value Details',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_report_activity(%(ActivityId)s, %(DescriptorId)s, %(DescriptorName)s, %(MetricId)s, %(Value)s, %(Details)s);',
	'cleanup': '',
	'final': ''
},
'create_descriptor': {
	'variables': 'Type DescriptorName',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_create_descriptor( global_unid, %(Type)s, %(DescriptorName)s);',
	'cleanup': '',
	'final': 'siox_release_descriptor( global_unid, %(Type)s, %(DescriptorName)s");'
},
'send_descriptor': {
	'variables': 'TargetSWID Type DescriptorName',
	'global': '''''',
	'init': '''''',
    'before': '''siox_send_descriptor( global_unid, "%(TargetSWID)s", "%(Type)s", "%(DescriptorName)s" );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'map_descriptor': {
	'variables': 'Descriptor From To',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_map_descriptor( global_unid, %(Descriptor)s, %(From)s, %(To)s);',
	'cleanup': '',
	'final': ''
},
'receive_descriptor': {
	'variables': 'Type Name',
	'global': '''''',
	'init': '''''',
    'before': '''siox_receive_descriptor( global_unid, %(Type)s, %(Name)s );''',
	'after': '',
	'cleanup': '',
	'final': ''
},
'release_descriptor': {
	'variables': 'Type DescriptorName',
	'global': '''''',
	'init': '''''',
    'before': '''''',
	'after': 'siox_release_descriptor( global_unid, %(Type)s, %(DescriptorName)s);',
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

ForEachBefore = ""

ForEachAfter = ""

throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

includes = ["<stdio.h>", "<stdlib.h>", "<hdf5.h>", "\"siox-ll.h\""]

dlsymLibPath = "\"/usr/local/\""

dllib = "\"libhdf5.dylib\""