template = {
'ontology': {
	'variables': 'VARNAME VARTYPE',
	'global': 'siox_dtid dtid%(VARNAME)s;',
	'init': 'dtid%(VARNAME)s = siox_register_datatype( %(VARNAME)s, %(VARTYPE)s );',
	'before': '',
	'after': '',
	'final': ''
},
'activity': {
	'variables': 'CALL',
	'global': '',
	'init': '',
	'before': 'siox_aid _aid%(CALL)s = siox_start_activity( unid, %(CALL)s );',
	'after': 'siox_stop_activity(_aid%(CALL)s);',
	'final': 'siox_end_activity(_aid%(CALL)s);'
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
