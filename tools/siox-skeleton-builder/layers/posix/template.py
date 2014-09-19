template = {
	'errorErrno': {
	        'variables': 'Condition="ret<0" Activity=sioxActivity',
	        'after': ''' int errsv = errno;
			    if ( %(Condition)s ){
			    				 siox_activity_stop(%(Activity)s);
	                      siox_activity_report_error( %(Activity)s, errsv );	                      
	                      siox_activity_end(%(Activity)s);
								 errno = errsv;
								 return ret;
	                    }''',
	},
	'POSIX_activity': {
		'variables': 'Name=%(FUNCTION_NAME)s ComponentActivity=cv%(FUNCTION_NAME)s ComponentVariable=global ActivityVar=sioxActivity Condition="ret<0"',
		'templates': ["@guard", "@errorErrno Condition=''%(Condition)s'' Activity=%(ActivityVar)s", "@activity Name=%(Name)s ComponentActivity=%(ComponentActivity)s ComponentVariable=%(ComponentVariable)s ActivityVariable=%(ActivityVar)s"]
	},
}
