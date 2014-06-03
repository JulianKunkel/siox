template = {
# test
#
# Writes a given message to stdout.
#
# Text: The text to print
'test': {
    'variables': '''Text=%s Text2=""''',
    'global': '''''',
    'init': '''''',
    'before': '''fprintf(stderr, "%%s (%(Text)s);\\n", __FUNCTION__, %(Text2)s);''',
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
}
}

templateParameters = {

# Insert global once
"globalOnce" : "",

# Regexes for functions to throw away
"throwaway" : ["((^\s*)|(\s+))extern\s+.*\("],

# Will be included
"includes" : ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>', '<C/siox.h>'],

}