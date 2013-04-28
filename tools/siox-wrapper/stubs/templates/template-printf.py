template = {
# test
#
# Writes a given message to stdout.
#
# Text: The text to print
'test': {
    'variables': '''Text="" Text2=""''',
    'global': '''''',
    'init': '''''',
    'before': '''printf("%%s (%(Text)s);\\n", __FUNCTION__, %(Text2)s);''',
    'after': '',
    'cleanup': '',
    'final': ''
}
}

# Insert global once
globalOnce = "static GMutex TestMapMutex; \n GSList *testlist = NULL;"

# Regexes for functions to throw away
throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

# Will be included
includes = ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>']
