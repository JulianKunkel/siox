template = {
# component
#
# Registers (and unregisters) a new component with SIOX and
# connects to the ontology.
#
# SWID: The name (software id) for this component
'component': {
	'variables': 'SWID',
	'global': '''''',
	'init': '''printf("Registering component with SWID %%s\\n", %(SWID)s);''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': 'printf("Unregistering component with SWID %%s\\n", %(SWID)s);'
},
# register_attribute
#
# Registers a new attribute type with siox, so that later calls
# can set values for this type of attribute on certain entities.
#
# AttributeVariable: Name of the variable which is used to store the
#		   			 pointer to the attribute type
# Name: Name of the attribute
# MinStorage: The minimum storage type required to store this
#			  attribute
'register_attribute': {
	'variables': 'Name',
	'global': '''''',
	'init': '''printf("Registering attribute with attribute's name %%s\\n", %(Name)s);''',
    'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# register_descriptor
#
# Registers a new descriptor type with siox, so that later calls
# can set values for this type of descriptor on certain entities.
#
# DescriptorVariable: Name of the variable which is used to store the
#		   			 pointer to the descriptor type
# Name: Name of the descriptor
# MinStorage: The minimum storage type required to store this
#			  descriptor
'register_descriptor': {
	'variables': 'Name',
	'global': '''''',
	'init': '''printf("Registering descriptor with descriptor's name %%s\\n", %(Name)s);''',
	'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_create_str
#
# Creates a map data structure to keep track of descriptors that can be represented as strings.
# These will be used for the horizontal linking of all activities using that descriptor via
# activity_link_str.
#
# MapName: The name for this map; defaults to activityHashTable_str
'horizontal_map_create_str': {
	'variables': 'MapName=activityHashTable_str',
	'global': '''''',
	'init': '''printf("Creating hash table of string type with name %%s\\n", "%(MapName)s");''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_create_int
#
# Creates a map data structure to keep track of descriptors that can be represented as
# either ints or long ints.
# These will be used for the horizontal linking of all activities using that descriptor via
# activity_link_int.
#
# MapName: The name for this map; defaults to activityHashTable_int
'horizontal_map_create_int': {
	'variables': 'MapName=activityHashTable_int',
	'global': '''''',
	'init': '''printf("Creating hash table of int type with name %%s\\n", "%(MapName)s");''',
	'before': '',
	'after': '',
	'cleanup': '',
	'final': ''
},
# register_metric
#
# Registers a new metric with SIOX.
#
# MetricVariable: Name of the variable to store the metric type in
# Name: Name of the metric
# UnitType: SIOX unit-type for the metric
# StorageType: SIOX storage-type for the metric
# ScopeType: SIOX scope-type for the metric
'register_metric': {
	'variables': 'RetName Name UnitType StorageType ScopeType',
	'global': '''''',
	'init': '''printf("Register Metric %%s with %%s RetName and %%s UnitType and %%s StorageType and %%s ScopeType\\n", %(Name)s, "%(RetName)s", "%(UnitType)s", "%(StorageType)s", "%(ScopeType)s");''',
	'before': '''''',
	'after': '',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_int
#
# Clear an attibute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_int.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
	'after': '''printf("Removing hash table with map name %%s of int type. Key is %%s and activity is %%s\\n", "%(MapName)s", "%(Key)s", "%(Activity)s");''',
	'cleanup': '',
	'final': ''
},
# horizontal_map_remove_str
#
# Clear an attibute serving as a descriptor.
# Linking activities by this attribute will now require a new call to horizontal_map_put_str.
# Attributes for activities are either its parameters or other values computed from them.
# Metrics or statistics resulting from the call use activity_report instead
#
# Key: The descriptor linking the activity to others, represented as an int
# MapName: The map to be used; defaults to activityHashTable_int
'horizontal_map_remove_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '',
	'after': '''printf("Removing hash table with map name %%s of string type. Key is %%s and activity is %%s\\n", "%(MapName)s", "%(Key)s", "%(Activity)s");''',
	'cleanup': '',
	'final': ''
},
# activity
#
# Starts (at the beginning) and stops (at the end) a new
# activity in the current function.
# Any metrics resulting from the activity still need to be reported via activity_report!
#
# Name: Short description of the activity
# Activity: Name of the variable to store the activity in; defaults to sioxActivity
# TimeStart: Start time to be reported; defaults to NULL, which will draw a current time stamp
# TimeStop: Stop time to be reported; defaults to NULL, which will draw a current time stamp
'activity': {
	'variables': 'AID=aid TimeStart=NULL TimeStop=NULL TimeEnd=NULL Name=__FUNCTION__',
	'global': '''''',
	'init': '''''',
	'before': '''printf("Creating activity %%s with %%s TimeStart and %%s name\\n", "%(AID)s", "%(TimeStart)s", %(Name)s);''',
	'after': '''printf("Stoping activity %%s with %%s TimeStop\\n", "%(AID)s", "%(TimeStop)s");''',
	'cleanup': 'printf("Ending activity %%s with %%s TimeEnd\\n", "%(AID)s", "%(TimeEnd)s");',
	'final': ''
},
# activity_link_int
#
# Horizontally links the current activity (started with activity) to another one via
# a desctriptor represented as either an int or a long int.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in int or long int form
# Activity: Activity to be linked; defaults to sioxActivity
'activity_link_int': {
	'variables': 'Key MapName=activityHashTable_int Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '''''',
	'after': '''printf("link activity with map name %%s of int type. Key is %%s and activity is %%s\\n", "%(MapName)s", "%(Key)s", "%(Activity)s");''',
	'cleanup': '',
	'final': ''
},
# activity_link_str
#
# Horizontally links the current activity (started with activity) to another one via
# a desctriptor represented as a string.
#
# Key: A descriptor linking both activities together (such as a file name),
#	   in string form
# Activity: Activity to be linked; defaults to sioxActivity
'activity_link_str': {
	'variables': 'Key MapName=activityHashTable_str Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '''''',
	'after': '''printf("Link activity with map name %%s of string type. Key is %%s and activity is %%s\\n", "%(MapName)s", "%(Key)s", "%(Activity)s");''',
	'cleanup': '',
	'final': ''
},
# error
#
# Reports that an activity terminated with an error
#
# Condition: A guard expression to be evaluated after the wrapped function call;
#			 if true, an error with code Error will be reported to SIOX, indicating
#			 that all metrics monitored on this call are tainted.
# Error: The error code to be reported; must be of type int
# Activity: The Activity to be reported; defaults to sioxActivity
'error': {
	'variables': 'Condition Error Activity=sioxActivity',
	'global': '''''',
	'init': '''''',
	'before': '''''',
	'after': '''if ( %(Condition)s )
	printf("ERROR!!");''', 
	'cleanup': '',
	'final': ''
},
# activity with hints
#
# Starts (at the beginning) and stops (at the end) a new
# activity in the current function and transfer the MPI hints to SIOX activity
# Any metrics resulting from the activity still need to be reported via activity_report!
#
# Name: Short description of the activity
# Activity: Name of the variable to store the activity in; defaults to sioxActivity
# TimeStart: Start time to be reported; defaults to NULL, which will draw a current time stamp
# TimeStop: Stop time to be reported; defaults to NULL, which will draw a current time stamp
'activity_with_hints': {
	'variables': 'AID=aid TimeStart=NULL TimeStop=NULL TimeEnd=NULL Name=__FUNCTION__',
	'global': '''''',
	'init': '''''',
	'before': '''printf("Creating activity %%s with %%s TimeStart and %%s name\\n", "%(AID)s", "%(TimeStart)s", %(Name)s);''',
	'after': '''printf("the __real_MPI_File_get_info() should be called here");
	printf("TODO: here should be a function to convert the info_used to Attribute:Value tuple in order to transfer the Hints to siox.");
	printf("TODO: before sending the Hints to siox, a checkout function should be called to filter the duplicated Hints.");
	printf("TODO: or shall we just leave the checkout to the siox activity?");
	printf("TODO: set the MPI info as activity attributes");
	printf("the __real_MPI_Info_free() should be called here");
	printf("Stoping activity %%s with %%s TimeStop\\n", "%(AID)s", "%(TimeStop)s");''',
	'cleanup': 'printf("Ending activity %%s with %%s TimeEnd\\n", "%(AID)s", "%(TimeEnd)s");',
	'final': ''
},
# test
#
# Writes a given message to stdout.
#
# Text: The text to print
'test': {
    'variables': '''Text="" Text2=""''',
    'global': '''''',
    'init': '''''',
    'before': '''printf("%%s (%(Text)s);\\n", G_STRFUNC, %(Text2)s);''',
    'after': '',
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
includes = ['<stdlib.h>', '<stdio.h>', '<stdarg.h>', '<glib.h>', '<mpi.h>']
