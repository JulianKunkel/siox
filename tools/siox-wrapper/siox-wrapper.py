#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8
from __future__ import print_function

globalOnce = ""

import re
import sys
import argparse
import subprocess


##
# @brief Generate and handle the command line parsing.
#
# This class is just a simple Wrapper of the python argparser.
class  Option():

    ##
    # @brief Parse the command line arguments.
    #
    # Parses the command line arguments, using the python argparser.
    def parse(self):
        prog = 'SIOX-Wrapper'

        description = '''The SIOX-Wrapper is a tool which instrument software
libraries, by calling trace functions before and after the actual library call.'''

        argParser = argparse.ArgumentParser(description=description, prog=prog)

        argParser.add_argument('--output', '-o', action='store', nargs=1,
        dest='outputFile', default=['out'], help='Provide a file to write the output.')

        argParser.add_argument('--blank-header', '-b', action='store_true',
                default=False, dest='blankHeader',
                help='''Generate a clean header file, which can be instrumented,
from a other header file.''')

        #argParser.add_argument('--cpp-args', '-a', action='store', nargs=1, default=[],
        #dest='cppArgs', help='''Pass arguments to the cpp.''')

        argParser.add_argument('--style', '-s',
        action='store', default='dlsym', dest='style',
        choices=['wrap', 'dlsym', 'plain'],
        help='''Choose which output-style to use.''')

        argParser.add_argument('inputFile', default=None,
        help='Source or header file to parse.')

        argParser.add_argument('--debug', '-d',
            action='store_true', dest='debug',
            help='Debug every line')

        argParser.add_argument('--output-wrap-file', '-W',
            action='store', dest='wrapFile',  help='File to store GCC wrap options')


        argParser.add_argument('--template', '-t',
            action='store', default='./template.py', dest='template',
            help='Provide an alternative template.')
        args = argParser.parse_args()

        if args.outputFile:
            args.outputFile = args.outputFile[0]

        # import the templates (symbol template)
        namespace = {}
        execfile(args.template, namespace)
        globals().update(namespace)

	# update the datastructures for the templates

	for t in template.keys():
		entries = template[t]
		if not "global" in entries:
			entries["global"] = ""
		entries["variablesDefaults"] = {}
		if not "variables" in entries:
			entries["variables"] = []
		else:
			var = re.split("[ \t,]+", entries["variables"])
			processedVars = []
			# extract default values:
			for v in var:
			   if "=" in v:
				# we have a default
				(name, default) = v.split("=")
				processedVars.append(name)
				entries["variablesDefaults"][name] = default
			   else:
			        processedVars.append(v)
			entries["variables"] =  processedVars

		if not "init" in entries:
			entries["init"] = ""
		if not "before" in entries:
			entries["before"] = ""
		if not "after" in entries:
			entries["after"] = ""
		if not "cleanup" in entries:
			entries["cleanup"] = ""
		if not "final" in entries:
			entries["final"] = ""

        return args

##
# @brief A storage class for a function.
#
# Stores a Function and offers functions for formatted output.
class Function():

    def __init__(self):

        ## Return type of the function (int*, char, etc.)
        self.type = ''
        ## Name of the function
        self.name = ''
        ## A list of Parameters, a parameter is a extra class for storing.
        self.parameterList = []
        ## A list of templates associated with the function.
        self.definition = ''
        self.usedTemplateList = []
        ## Indicates that the function is the first called function of the
        # library and initialize SIOX.
        self.init = False
        ## Indicates that the function is the last called function of the
        # library.
        self.final = False

    ##
    # @brief Generate the function call.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the function call.
    #
    # Generates the function call of the function object.
    def getCall(self):

        if len(self.parameterList) == 1:
            if self.parameterList[0].name == 'void':
                return '%s()' % (self.name)

        return '%s(%s)' % (self.name,
         ', '.join(parameter.name for parameter in self.parameterList))

    ##
    # @brief Generate the function definition.
    #
    # @param self The reference to this object.
    #
    # @return The function definition as a string.
    #
    # Generates the function call of the function object.
    def getDefinition(self):

        if self.definition == '':
            return '%s %s(%s)' % (self.type, self.name,
                ', '.join(' '.join([parameter.type, parameter.name])
                for parameter in self.parameterList))

        else:
            return '%s %s%s' % (self.type, self.name, self.definition)

    ##
    # @brief Generate the function call prefixed with __real_.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the the function call prefixed with __real_.
    #
    # Generates the function call of the function object for the gcc wrap
    # functionality. The Call will be prefixed with __real_ and used as the
    # original function.
    def getCallReal(self):

        if len(self.parameterList) == 1:
            if self.parameterList[0].name == 'void':
                return '__real_%s()' % (self.name)
        return '__real_%s(%s)' % (self.name,
         ', '.join(parameter.name for parameter in self.parameterList))

    ##
    # @brief Generate the function definition prefixed with __real_.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the function definition prefixed with __real_.
    #
    # Generates the function definition of the function object for the gcc wrap
    # functionality. The definition will be prefixed with __real_.
    def getDefinitionReal(self):

        if self.definition == '':
            return '%s __real_%s(%s)' % (self.type, self.name,
                ', '.join(' '.join([parameter.type, parameter.name])
                for parameter in self.parameterList))

        else:
            return '%s __real_%s %s' % (self.type, self.name, self.definition)

    ##
    # @brief Generate the function definition prefixed with __wrap_.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the function definition prefixed with __real_.
    #
    # Generates the function definition of the function object for the gcc wrap
    # functionality. The definition will be prefixed with __real_.
    def getDefinitionWrap(self):

        if self.definition == '':
            return '%s __wrap_%s(%s)' % (self.type, self.name,
                ', '.join(' '.join([parameter.type, parameter.name])
                for parameter in self.parameterList))

        else:
            return '%s __warp_%s%s' % (self.type, self.name, self.definition)

    ##
    # @brief Generate the function call with a function pointer for dlsym.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the function call for a function pointer.
    #
    # Generate the function pointer call for dlsym. The function
    # pointer call looks something like: (__real_funcName)(parameter).
    def getCallPointer(self):

        if len(self.parameterList) == 1:
            if self.parameterList[0].name == 'void':
                return '(__real_*%s)()' % (self.name)
        return '(__real_%s)(%s)' % (self.name,
            ', '.join(parameter.name for parameter in self.parameterList))

    ##
    # @brief Generate the function pointer definition for dlsym.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the pointer definition of the function.
    #
    # Generate the function pointer definition for dlsym. The function pointer
    # definition looks something like: funcType (*_real_funcName)(parType parName).
    def getDefinitionPointer(self):

        if self.definition == '':
            parameters = ', '.join(' '.join([parameter.type, parameter.name])
                for parameter in self.parameterList)

            return ('static %s (*__real_%s) (%s);' % (self.type, self.name, parameters))

        else:

            return ('static %s (*__real_%s) (%s);' % (self.type, self.name,
                self.definition))

    ##
    # @brief Generate the C code to open the function with dlsym.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the dlsym call.
    #
    # Generate the C code for the dlsym function call.
    def getDlsym(self):

        if self.definition == '':

            parameters = ', '.join(' '.join([parameter.type, parameter.name])
                for parameter in self.parameterList)

            return ('__real_%s = (%s (*) (%s)) dlsym(dllib, (const char*) "%s");' %
                (self.name, self.type, parameters, self.name))

        else:

            return ('__real_%s = (%s (*) (%s)) dlsym(dllib, (const char*) "%s");' %
                (self.name, self.type, self.definition, self.name))

    ##
    # @brief Generate an identifier of the function.
    #
    # @param self The reference to this object.
    #
    # @return A unique identifier for the function as a string.
    #
    # The generated identifier is used as a key for the hash table the functions
    # are stored in.
    def getIdentifier(self):

        identifier = '%s%s(%s);' % (self.type, self.name,
                ''.join(''.join([parameter.type, parameter.name]) for
                    parameter in self.parameterList))

        return re.sub('[,\s]', '', identifier)


##
# @brief One parameter of a function.
#
# This class holds only data.
class Parameter():

    def __init__(self):
        ## The type of the parameter.
        self.type = ''
        ## The name of the parameter.
        self.name = ''


##
# @brief An instruction form an instrumented header file.
#
# The instructions a read form a instrumented header file. The instruction
# names are the entries found in the template file.
class Instruction():

    ##
    # @brief Constructor for the Instruction class.
    #
    # @param self The reference to this object.
    def __init__(self):
        ## Name of the instruction.
        self.name = ''
        ## Parameters of the instruction.
        self.parameterList = []


##
# @brief Parses a header file and looks for function definitions.
#
# The function parser searches through a header file using regular expressions.
class FunctionParser():

    ##
    # @brief Constructor for the FunctionParser class.
    #
    # @param self The reference to this object.
    #
    # @param options An Option object.
    def __init__(self, options):

        self.inputFile = options.inputFile
        #self.cppArgs = options.cppArgs
        self.blankHeader = options.blankHeader

        ## This regular expression searches for the general function definition.
        # ([\w*\s]) matches the return type of the function.
        # (?:\s*\w+\s*\()) is a look ahead that ensures that the first part of
        # the regex ends right before the function name. The look ahead searches for
        # last 'word' right in front of the (.
        # the \s*(\w+)\s* actually matches the function name (a look ahead
        # doesn't consume text).
        # \(([,\w*\s\[\]]*)\) matches the parentheses of the function definition
        # and groups everything inside them. The regex can't match single
        # parameters because a regex must have a fixed number of groups to match.
        self.regexFunctionDefinition = re.compile('(?:([\w*\s]+?)(?=\s*\w+\s*\())\s*(\w+)\s*\(([,\w*\s\[\].]*)\)[\w+]*?;',
        re.S | re.M)

        ## This regular expression matches parameter type and name.
        # The Parameter which is matched needs to have a type and a name and is
        # only used when the C code of the instrumented header file is
        # generated. The instrumented header must provide the type and the name
        # of every parameter.
        # [\w*\s]+ matches everything until the last * or space.
        # (?:\*\s*|\s+) matches the last * or blank
        # ([\w]+ matches the parameter name
        # (?:\s*\[\s*\])? matches array [] if exist
        self.regexParamterDefinition = re.compile('([\w*\s]+(?:\*\s*|\s+))([\w]+(?:\s*\[\s*\])?)')

        ## This tuple of filter words searches for reseverd words in the
        ## function return type.
        # Some typedefs can look like function a definition the regex.
        self.filter = ('typedef', '//', '#', 'return')

    ##
    # @brief This function parses the header file.
    #
    #
    # @param self The reference to the object.
    #
    # @param string A string to parse for function.
    #
    # @return A list of function definitions.
    #
    # This function dose the actually searching for functions inside a string.
    # The string can contain multiple function in one string.
    def parse(self, string):

        functionList = []

        # Match all function definitions
        iterateFunctionDefinition = self.regexFunctionDefinition.finditer(string)

        for functionDefinition in iterateFunctionDefinition:
            filtered = False

            for element in self.filter:
                if functionDefinition.group(1).find(element) != -1:
                    filtered = True

            # At this point we are quite certain, we found a function definition

            if filtered is True:
                continue

            function = Function()

            # Extract the type an name form the match object.
            function.type = functionDefinition.group(1).strip()
            function.name = functionDefinition.group(2).strip()

            # Get the parameter string.
            parameterString = functionDefinition.group(3).strip()
            # Substitute newlines and multiple blanks
            parameterString = re.sub('\n', '', parameterString)
            parameterString = re.sub('\s+', ' ', parameterString)

            # If only the blank header file should be generated pass the
            # string of parameters to the Function object.
            if self.blankHeader:
                    function.definition = "(" + parameterString + ")"
            # If the C source code should be generated split the string into a
            # list of parameters and extract the type and name of the parameter.
            else:
                parameterList = parameterString.strip().split(',')

                for parameter in parameterList:
                    parameterObject = Parameter()
                    parameter = parameter.strip()

                    if parameter in ('void', '...', ''):
                        parameterName = ''
                        parameterType = parameter

                    else:
                        parameterMatch = self.regexParamterDefinition.match(parameter)
                        parameterType = parameterMatch.group(1)
                        parameterName = parameterMatch.group(2)

                        # Search for something like 'int list[]' and convert it to
                        # 'int* list'
                        regexBracketes = re.compile('\[\s*\]')
                        if regexBracketes.search(parameterName):
                            parameterName = regexBracketes.sub('', parameterName)
                            parameterType += '*'

                    parameterObject.name = parameterName
                    parameterObject.type = parameterType
                    function.parameterList.append(parameterObject)

            functionList.append(function)
        return functionList

    ##
    # @brief Wrapper function for parse().
    #
    # @return A list of function objects
    #
    # @param self
    #
    # This is a wrapper function for the parse(). parseFile takes the input
    # header file form the option object and pass it to cpp. The output off cpp
    # is then passed to the function parse.
    def parseFile(self):

        string = ''

        fh = open(self.inputFile)
        string = fh.read()
        functionList = self.parse(string)
        return functionList

    # @brief Wrapper function for parse()
    #
    # @return A list of function objects
    #
    # @param self The reference to this object.
    #
    # param string The string to be parsed.
    def parseString(self, string):

        functionList = self.parse(string)
        return functionList


##
# @brief The command parser which reads the instrumentation instructions.
#
# The command parser reads the instrumentation form a commented header file.
# The parsing is based on regular expressions.
class CommandParser():

    def __init__(self, options):
        self.inputFile = options.inputFile
        ## This regular expression matches the instructions which begin with //
        self.commandRegex = re.compile('^\s*//\s*@(\w+)\s*(.*)')
        self.includeRegex = re.compile('^\s*#\s*include\s*([-.<>\"\w\'/]+)\s*')
        self.options = options

    ##
    # @brief This function parses the header file.
    #
    # @param self The reference to the object.
    #
    # @param functions The list of function objects returned by the function
    # parser.
    #
    # @return A list of function objects which are extended by the
    # instrumentation instructions.
    #
    # Parses a instrumented header file and adds the instrumentation instructions
    # to the functions inside the list of functions an returns the new list.
    def parse(self):
        functionParser = FunctionParser(self.options)

	# create default values for templates.


        availableCommands = template.keys()
        input = open(self.inputFile, 'r')
        inputString = input.read()
        functionString = ""
        templateList = []
        functionList = []

        # Strip comments
        inputString = re.sub('/\*.*?\*/', '', inputString, flags=re.M|re.S)

        inputLineList = inputString.split('\n')
        # Iterate over every line and search for instrumentation instructions.
        i = 0
        try:
            while i < len(inputLineList):

                commandFinished = False
                functionFinished = False
                currentFunction = Function()

                # match the line against the include regex
                include = self.matchInclude(inputLineList[i])
                if include and include not in includes:
                    if self.options.debug:
                        print("New include '%s' at line %i" % (include, i))
                    includes.append(include)
                    functionString = ""
                    i += 1

                # Because a instrumentation command can be longer than one line we
                # we have to insure to read the whole command.
                while not commandFinished:

                    commandFinished = self.matchCommand(inputLineList[i], templateList)
                    if not commandFinished:
                        functionString = ""
                        if self.options.debug:
                            print("Instrumention command '%s' at line %i"
                                % (templateList[-1].name, i))
                        i += 1
                        functionString = ""

                functionString += inputLineList[i]
                if self.options.debug:
                    print("Scanning '%s' for functions at line %i" % (functionString, i))

                # Try to parse the inputLine as a function string
                currentFunctionList = functionParser.parseString(functionString)
                if len(currentFunctionList) > 0:

                    currentFunction = currentFunctionList[0]
                    if self.options.debug:
                        print("Function '%s' at line %i" % (currentFunction.name, i))
                    currentFunction.usedTemplateList = templateList[:]
                    functionList.append(currentFunction)
                    templateList = []
                    functionString = ""
                i+=1
        except IndexError:
            pass

        if self.options.debug:
            print("Finished scanning input header file.")
            if len(templateList) > 0:
                print("""WARNING: Found unassociated instrumentation commands\
at the end of """)
        return functionList

    def matchInclude(self, inputLine):

        match = self.includeRegex.match(inputLine)
        if match:
            include = match.group(1).strip()
            return include
        else:
            return False

    ##
    # @brief This function matches a instruction command with the prefix //@
    #
    def matchCommand(self, inputLine, templateList):

        match = self.commandRegex.match(inputLine)

        if (match):
            availableCommands = template.keys()
            commandName = match.group(1)
            commandArgs = match.group(2) + " "

            # Search for init or final sections to define init or final
            # functions
            if commandName == 'init':
                currentFunction.init = True

            elif commandName == 'final':
                currentFunction.fnal = True

            # If the current instrumentation command is available in the template
            # create a new template object and fill it with the command data
            elif commandName in availableCommands:
                commandName = match.group(1)
                commandArgs = match.group(2) + " "
                newTemplate = Template(template[commandName], commandName, commandArgs)
                templateList.append(newTemplate)

            # If the command name is not in the template the parsed line
            # belongs to the command parsed in a earlier line and the line only
            # contains command arguments.
            else:
                templateList[-1].commandArgs = commandName +\
                    " " + commandArgs

            return False

        else:
            return True

##
# @brief A template function including parameters and arguments
class Template():
    ##
    # @brief The constructor
    #
    # @param name The name of the used template
    # @param variables The used variables
    def __init__(self, templateDict, name, variables):

        self.templateDict = templateDict;
        self.name = name
        self.parameterList = {}

        # This regex parses the values string in the function setParameters.
        # It distinguishes between 3 cases:
        # match the first bare word of the values
        # match double quoted strings
        # match single quoted strings
	# Additionally, it is allowed to prefix the assignment with <variableName>=
        self.valueRegex = re.compile('\s*(\w+=)?(([-\w%_\(\)\[\]&*]+)|(\".*?\")|(\'.+?\'+))\s*', re.S | re.M)

        # Generate strings for output from given input
        self.setParameters(self.templateDict['variables'], self.templateDict['variablesDefaults'], variables)

        # Remember template-access for easier usage
        self.world = self.templateDict['global']
        self.init = self.templateDict['init']
        self.before = self.templateDict['before']
        self.after = self.templateDict['after']
        self.final = self.templateDict['final']
        self.cleanup = self.templateDict['cleanup']

    ##
    # @brief Reads the parameters and generates the needed output
    #
    # @param names The name of the used template
    # @param values The used variables
    def setParameters(self, nameList, default_value_dictionary, values):
        # TODO: raise error and error handling
        # generate a list of all names and values

	# initialize variables, this is needed for named parameters
        for name in nameList:
            if name in default_value_dictionary:
               self.parameterList[name] = default_value_dictionary[name]
            else:
                self.parameterList[name] = ""

        for name in nameList:
            # Match the next value and only one value out of the string
            value = self.valueRegex.match(values)
            if value:
		if(value.group(1)):
			name = value.group(1).strip("=")
			# the variable with the given name is set
			if not name in nameList:
				print("Error name " + name + " is not part of the defined function " + self.name)
				exit(1)
                # Set the matched value
                valueString = value.group(2).strip()
                if(valueString.startswith("''")):
                	valueString = valueString.strip("'")
                self.parameterList[name] = valueString

                # Truncate the found value from the value string
                values = self.valueRegex.sub('', values, 1)

        # All further text belongs to the last parameter.
        lastName = nameList[-1]
	self.parameterList[lastName] += " " + values.strip()

    ##
    # @brief Used for selective output
    #
    # @param type What part of the template should be given
    #
    # @return The requested string from the template
    def output(self, type):
        if (type == 'global'):
            return self.world % self.parameterList
        elif (type == 'init'):
            return self.init % self.parameterList
        elif (type == 'before'):
            return self.before % self.parameterList
        elif (type == 'after'):
            return self.after % self.parameterList
        elif (type == 'final'):
            return self.final % self.parameterList
        elif (type == 'cleanup'):
            return self.cleanup % self.parameterList
        else:
            # Error
            print('ERROR: Section: ', type, ' not known.', file=sys.stderr)
            sys.exit(1)


##
# @brief The output class (write a file to disk)
#
class Writer():

    ##
    # @brief The constructor
    #
    # @param options The supplied arguments
    def __init__(self, options):
        self.outputFile = options.outputFile
	self.wrapFile = options.wrapFile
	self.writeWrapFile = (options.style == "wrap")

    ##
    # @brief Write a header file
    #
    # @param functions A list of function-objects to write
    def headerFile(self, functionList):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all function headers
        for function in functionList:
            for match in throwaway:
                if re.search(match, function.getDefinition()):
                    function.name = ''

            if function.type == '':
                continue

            elif function.name == '':
                continue

            else:
                print(function.getDefinition(), end=';\n', sep='', file=output)

        # close the file
        output.close()

    ##
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def sourceFileWrap(self, functionList):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all needed includes
        for match in includes:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)

        # write all global-Templates
        for function in functionList:
            for templ in function.usedTemplateList:
                if templ.output('global') != '':
                    print(templ.output('global'), file=output)
        print("", file=output)

        # write global once from template
        print(globalOnce, file=output)

        print("static void sioxFinal() __attribute__((destructor));", file=output)
        print("static void sioxInit() __attribute__((constructor));", file=output)

        # write all function redefinitions
        for function in functionList:
            print(function.getDefinitionReal(), end=';\n', sep='', file=output)

        print("", file=output)

        print("static void sioxInit() {", file=output)
        # write all init-templates
        for function in functionList:
            for templ in function.usedTemplateList:
                outputString = templ.output('init').strip()
                if outputString.strip() != '':
                    print('\t', outputString, end='\n', sep='', file=output)
        print("}", file=output)

        print("static void sioxFinal() {", file=output)
        # write all final-functions
        for function in functionList:
            for templ in function.usedTemplateList:
                outputString = templ.output('final').strip()
                if outputString.strip() != '':
                    print('\t', outputString, end='\n', sep='', file=output)
        print("}", file=output)

        # write all functions-bodies
        for function in functionList:
            # write function signature

            print(function.getDefinitionWrap(),
                    end='\n{\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print('\tva_list args;', file=output)
                print('\tva_start(args, %s);' % function.parameterList[-2].name,
                        file=output)
                print('\t%s val = va_arg(args, %s);' % (function.parameterList[-2].type,
                    function.parameterList[-2].type), file=output)

                # set the name to args
                function.parameterList[-1].name = "val"

            # a variable to save the return-value
            returnType = function.type

            if returnType != "void":
                print('\t', returnType, ' ret;', end='\n', sep='',
                        file=output)

            # write the before-template for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('before').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write the function call
            if returnType != "void":
                print('\tret = ', function.getCallReal(), end=';\n', sep='',
                        file=output)
            else:
                print('\t', function.getCallReal(), end=';\n', sep='',
                        file=output)

            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('after').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write all cleanup-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('cleanup').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print("\tva_end(args);", file=output)

            # write the return statement and close the function
            if returnType != "void":
                print('\treturn ret;\n}', end='\n\n', file=output)

            else:
                print('\n}', end='\n\n', file=output)

        output.close()

        # generate gcc string for the user
	if self.wrapFile and self.writeWrapFile :
	        output = open(self.wrapFile, 'w')
	        gccHelper = '-Wl'

        	for function in functionList:
	            gccHelper = "%s,--wrap=%s" % (gccHelper, function.name)

        	print(gccHelper, file=output)
	        output.close()

    ##
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def sourceFilePlain(self, functionList):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all needed includes
        for match in includes:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)

        # write all global-Templates
        for function in functionList:
            for templ in function.usedTemplateList:
                if templ.output('global') != '':
                    print(templ.output('global'), file=output)
        print("", file=output)

        # write global once from template
        print(globalOnce, file=output)

        # write all functions-bodies
        for function in functionList:
            # write function signature

            print(function.getDefinition(),
                    end='\n{\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print('\tva_list args;', file=output)
                print('\tva_start(args, %s);' % function.parameterList[-2].name,
                    file = output)
                print('\t%s val = va_arg(args, %s);' % (function.parameterList[-2].type,
                    function.parameterList[-2].type), file=output)
                # set the name to args
                function.parameterList[-1].name = "val"

            # a variable to save the return-value
            returnType = function.type

            if returnType != "void":
                print('\t', returnType, ' ret;', end='\n', sep='',
                        file=output)

            # write the before-template for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('before').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('after').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print("\tva_end(args);", file=output)

            # write the return statement and close the function
            if returnType != "void":
                print('\treturn ret;\n}', end='\n\n', file=output)

            else:
                print('\n}', end='\n\n', file=output)
        # close the file
        output.close()

    ##
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def sourceFileDLSym(self, functionList):

        # open the output file for writing
        output = open(self.outputFile, 'w')

        print('#define _GNU_SOURCE', file=output)
        print('#include <dlfcn.h>\n', file=output)

        # write all needed includes
        for match in includes:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)

        print("static void sioxFinal() __attribute__((destructor));", file=output)
        print("static void sioxInit() __attribute__((constructor));", file=output)

        # write all global-Templates
        for function in functionList:
            for templ in function.usedTemplateList:
                if templ.output('global') != '':
                    print(templ.output('global'), file=output)
        print("", file=output)

        # write global once from template
        print(globalOnce, file=output)

        print("static char* dllib = RTLD_NEXT;", file=output)

        for function in functionList:
            print(function.getDefinitionPointer(), file=output)

        print("\nstatic void sioxInit() {\n", file=output)
        # write all init-templates
        for function in functionList:
            for templ in function.usedTemplateList:
                outputString = templ.output('init').strip()
                if outputString.strip() != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            print(function.getDlsym(), file=output)
        print("}", file=output)

        print("\nstatic void sioxFinal() {", file=output)
        # write all final-functions
        for function in functionList:
            for templ in function.usedTemplateList:
                outputString = templ.output('final').strip()
                if outputString.strip() != '':
                    print('\t', outputString, end='\n', sep='', file=output)

        print("}", file=output)

        for function in functionList:

            # write function signature
            print(function.getDefinition(), end='\n{\n', sep=' ',
                file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print('\tva_list args;', file=output)
                print('\tva_start(args, %s);' % function.parameterList[-2].name,
                    file = output)
                print('\t%s val = va_arg(args, %s);' % (function.parameterList[-2].type,
                    function.parameterList[-2].type), file=output)
                # set the name to args
                function.parameterList[-1].name = "val"

            # a variable to save the return-value
            returnType = function.type

            if returnType != "void":
                print('\t', returnType, ' ret;', end='\n', sep='',
                        file=output)

            # write the before-template for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('before').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write the function call
            if returnType != "void":
                print('\tret = ', function.getCallPointer(), end=';\n',
                    sep='', file=output)
            else:
                print('\t', function.getCallPointer(), end=';\n', sep='',
                        file=output)

            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('after').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('cleanup').strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print("\tva_end(args);", file=output)

            # write the return statement and close the function
            if returnType != "void":
                print('\treturn ret;\n}', end='\n\n', file=output)

            else:
                print('\n}', end='\n\n', file=output)

        # close the file
        output.close()


##
# @brief The main function.
#
def main():

  # Parse input parameter.
    opt = Option()
    options = opt.parse()

    functionParser = FunctionParser(options)
    outputWriter = Writer(options)

    if options.blankHeader:
        functions = functionParser.parseFile()
        outputWriter.headerFile(functions)

    else:
        commandParser = CommandParser(options)

        functions = commandParser.parse()
        if options.style == "wrap":
            outputWriter.sourceFileWrap(functions)
        elif options.style == "dlsym":
            outputWriter.sourceFileDLSym(functions)
        else:
            outputWriter.sourceFilePlain(functions)

if __name__ == '__main__':
    main()
