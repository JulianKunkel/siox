template = {
	'errorErrno': {
	        'variables': 'Condition="ret<0" Activity=sioxActivity',
	        'afterFirst': '''''',
	        'after': ''' ''',
	},
	'POSIX_activity': {
		'variables': 'Name=%(FUNCTION_NAME)s ComponentActivity=cv%(FUNCTION_NAME)s ComponentVariable=global ActivityVar=sioxActivity Condition="ret<0"',
		'templates': ["@guard", "@errorErrno Condition=''%(Condition)s'' Activity=%(ActivityVar)s", "@activity Name=%(Name)s ComponentActivity=%(ComponentActivity)s ComponentVariable=%(ComponentVariable)s ActivityVariable=%(ActivityVar)s"]
	},
}
