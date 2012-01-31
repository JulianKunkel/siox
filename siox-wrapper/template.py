# Aufbau: template['beschreibung']['before/after/init'];

includes = [
	'#import "siox_hh"'
]

variables = [
	'siox_unid global_unid;',
	'siox_dmid global_dmid;',
	'char[255] global_PID;',
	'char[255] global_SWID',
	'char[255] global_HWID'
]

template = {
'reg_node': {
	'init': 'global_HWID = SIOX_get_HWID();\n\
		 global_SWID = SIOX_get_SWID();\n\
		 global_PID = getPID();\n\
		 global_unid = SIOX_register_node(global_SWID, global_PID, global_HWID);\n',
	'before': '',
	'after': ''
},
'register_map': {
	'init': 'global_dmid = SIOX_register_descriptor_map(global_unid, "FileName", "FileHandle");',
	'before': '',
	'after': ''
},
'map': {
	'init': '',
	'before': 'SIOX_map_descriptor(global_unid, "FileName", filename, "FileHandle", ret);',
	'after': ''
}
}
