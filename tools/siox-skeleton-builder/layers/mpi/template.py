template = {
	'MPI_error': {
	'variables': 'Error=ret Activity=sioxActivity',
	'after': '''if ( ret!=MPI_SUCCESS ){
                      siox_activity_report_error( %(Activity)s, %(Error)s );
                      siox_activity_stop(%(Activity)s);
                      siox_activity_end(%(Activity)s);
                      return ret;
		    }''',
	},
}

