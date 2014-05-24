template = {
# register_node
#
# Registers (and ungeristers) a new node with SIOX and sets the ontology.
#
# SWID: The name (software id) for this node
'printf': {
    'variables': 'text',
    'global': '''''',
    'init': '''''',
    'before': 'printf(%(text)s);',
    'after': '',
    'cleanup': '',
    'final': ''
}
}

templateParameters = {
# Inserted before every call
"forEachBefore" : "",

# Inserted after every call
"forEachAfter" : "",

# Regexes for functions to throw away
"throwaway" : ["((^\s*)|(\s+))extern\s+.*\("],

# Will be included
"includes" : ['<stdlib.h>', '<stdarg.h>'],

}