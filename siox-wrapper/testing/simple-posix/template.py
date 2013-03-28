template = {
# register_node
#
# Registers (and ungeristers) a new node with SIOX and sets the ontology.
#
# SWID: The name (software id) for this node
'register_node': {
    'variables': 'SWID',
    'global': '''''',
    'init': '''printf("Registering Node with SWID %%s\\n", %(SWID)s);''',
    'before': '',
    'after': '',
    'cleanup': '',
    'final': 'printf("Unregistering Node with SWID %%s\\n", %(SWID)s);'
},
# metric_register
#
# Registers a new metric with SIOX
#
# RetName: Name of the variable to store the mid
# Name: Name of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'metric_register': {
    'variables': 'RetName Name UnitType StorageType ScopeType',
    'global': '''''',
    'init': '''printf("Register Metric %%s with %%s RetName and %%s UnitType and %%s StorageType and %%s ScopeType\\n", %(Name)s, "%(RetName)s", "%(UnitType)s", "%(StorageType)s", "%(ScopeType)s");''',
    'before': '''''',
    'after': '',
    'cleanup': '',
    'final': ''
},
# activity
#
# Starts (at the beginning) and stops (at the end) a new activity in the current function.
# Still needs to be reported!
#
# AID: Name of the varibale to store the aid
# TimeStart: Start time to be reported
# TimeStop: Stop time to be reported
# TimeEnd: End time to be reported
# name: Short description of the activity
'activity': {
    'variables': 'AID=aid TimeStart=NULL TimeStop=NULL TimeEnd=NULL Name=__FUNCTION__',
    'global': '''''',
    'init': '''''',
    'before': '''printf("Creating activity %%s with %%s TimeStart and %%s name\\n", "%(AID)s", "%(TimeStart)s", %(Name)s);''',
    'after': '''printf("Stoping activity %%s with %%s TimeStop\\n", "%(AID)s", "%(TimeStop)s");''',
    'cleanup': 'printf("Ending activity %%s with %%s TimeEnd\\n", "%(AID)s", "%(TimeEnd)s");',
    'final': ''
},
# activity_link
#
# Links the current activity (started with activity) to another one.
#
# AID: AID of the activity to be linked
# Key: Short description of the activity
'activity_link': {
    'variables': 'AID Key',
    'global': '''''',
    'init': '''''',
    'before': '''''',
    'after': '''printf("Linking activity %%s with %%s Key\\n", "%(AID)s", %(Key)s);''',
    'cleanup': '',
    'final': ''
},
# report
#
# Reports an activity
#
# UNID: The unid
# MID: Metric-ID of the reported value
# Value: Pointer to the value to be reported
'report': {
    'variables': 'UNID MID Value',
    'global': '''''',
    'init': '''''',
    'before': '''''',
    'after': 'printf("Reporting with UNID %%s and %%s MID and %%s Value\\n", "%(UNID)s", "%(MID)s", "%(Value)s");',
    'cleanup': '',
    'final': ''
}
}

# Inserted before every call
forEachBefore = ""

# Inserted after every call
forEachAfter = ""

# Regexes for functions to throw away
throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

# Will be included
includes = ['<stdlib.h>', '<stdarg.h>']
