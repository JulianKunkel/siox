template = {
# Writes a given message to stdout and saves it to a list
#
# Text: The text to print and save
'test': {
    'variables': '''Text=%s Text2=""''',
    'global': '''''',
    'init': '''''',
    'after': '''char*buff= malloc(512); snprintf(buff, 512, "%%s (%(Text)s)", __FUNCTION__, %(Text2)s);
    			 g_mutex_lock (&TestMapMutex);
    			 testlist = g_slist_append (testlist, buff);
    			 g_mutex_unlock (&TestMapMutex);''',
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

# Insert global once
globalOnce = "static GMutex TestMapMutex; \n static GSList *testlist = NULL; \n GSList * get_activity_list(){return testlist;}\n"

# Regexes for functions to throw away
throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

# Will be included
includes = ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>', '<C/siox.h>']
