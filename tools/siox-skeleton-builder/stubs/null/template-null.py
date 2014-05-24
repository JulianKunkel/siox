template = {
# test
#
# Writes a given message to stdout.
#
# Text: The text to print
'test': {
    'variables': '''nil''',
    'global': '''''',
    'init': '''''',
    'before': '''''',
    'after': '',
    'cleanup': '',
    'final': ''
},
'null': {
    'variables': '''retval=NULL''',
    'global': '''''',
    'init': '''''',
    'before': '''''',
    'after': 'ret = %(retval)s;',
    'cleanup': '',
    'final': ''
},
}

templateParameters = {
# Insert global once
"globalOnce" : "",

# Regexes for functions to throw away
"throwaway" : ["((^\s*)|(\s+))extern\s+.*\("],

# Will be included
"includes" : ['<stdlib.h>', '"C/siox.h"'],

}