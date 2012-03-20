class template():
	def __init__(self, templateDict, variables):
		self.name = templateDict['name']
		self.parameters = {}
		self.setParameters(templateDict['variables'], variables)
		self.global = templateDict['global']
		self.init = templateDict['init']
		self.before = templateDict['before']
		self.after = templateDict['after']
		self.final = templateDict['final']

	def setParameters(self, parameters, variables):
		parameterlist = paramters.split(' ')
		namelist = variables.split(' ')

		j = 0;
		for i in paramterlist:
			parameters[namelist[j]] = i
			j++

	def output(self, type):
		if (type == 'global'):
			return global % parameters
		elif (type == 'init'):
			return init % parameters
		elif (type == 'before'):
			return before % parameters
		elif (type == 'after'):
			return after % parameters
		elif (type == 'final'):
			return final % parameters
		else
			return 'Idiot'


ontology = {
	name: 'ontology',
	variables: 'VARNAME VARTYPE',
	global: 'siox_dtid dtid%(VARNAME)s;',
	init: 'dtid%(VARNAME)s = siox_register_datatype( %(VARNAME)s, %(STORAGETYPE)s );',
	before: '',
	after: '',
	final: ''
}

//ontology a b
ParsedString = 'a b'
iterArray = []
iterArray.add(template(ontology, parsedstring))
