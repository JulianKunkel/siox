template = {
	'MPI_activity': {
		'variables': 'ActivityVar=sioxActivity',
		'templates': ["@activity ActivityVariable=%(ActivityVar)s", "@error ''ret!=MPI_SUCCESS'' "]
	},
	'MPI_error': { # TODO REMOVE THIS ONE BECAUSE IT IS NOW MERGED WITH MPI_ACTIVITY
	'variables': 'Error=ret Activity=sioxActivity',
	'after': '''if ( ret!=MPI_SUCCESS ){
                      siox_activity_report_error( %(Activity)s, %(Error)s );
                      siox_activity_end(%(Activity)s);
                      return ret;
		    }''',
	},
	'MPI_comm_handle':{
		'variables': 'Comm=comm Activity=sioxActivity Attribute=commHandler',
		'before': '''{ 
			uint64_t commHandlerValue = getCommHandle(%(Comm)s); 
			siox_activity_set_attribute( %(Activity)s, %(Attribute)s, & commHandlerValue );
			}'''
	},
	'MPI_comm_handle_late':{
		'variables': 'Comm=comm Activity=sioxActivity Attribute=commHandler',
		'after': '''{ 
			uint64_t commHandlerValue = getCommHandle(%(Comm)s); 
			siox_activity_set_attribute( %(Activity)s, %(Attribute)s, & commHandlerValue );
			}'''
	},
	'MPI_file_createMap': {
		'variables': 'MapName=activityHashTable_size',
		'templates': ["@horizontal_map_create_size MapName=%(MapName)s"]
	},	
	'MPI_communication_activity': {
		'variables': 'ActivityVar=sioxActivity',
		'templates': ["@activity ActivityVariable=%(ActivityVar)s ComponentVariable=comm", "@error ''ret!=MPI_SUCCESS'' "]
	},	
	'MPI_comm_handler':{
		'variables': 'Comm=comm',
		'templates': ["@splice_before ''uint64_t commHandlerValue = getCommHandle(%(Comm)s);''", "@activity_attribute commHandler commHandlerValue" ]
		},
	'MPI_comm_handler_out':{
		'variables': 'Comm=comm',
		'templates': ["@splice_after ''uint64_t commHandlerValueOut = getCommHandle(%(Comm)s);''", "@activity_attribute_late commHandlerOut commHandlerValueOut" ]
	}
}

