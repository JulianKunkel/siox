#!/usr/bin/python2
# -*- coding: utf-8 -*-
# encoding: utf-8
# JK 2013, based on siox-wrapper

# This program creates a dummy "plugin" of a SIOX component.

from __future__ import print_function

import re
import sys
import argparse

componentFunctions = "\nvirtual void init(core::ComponentOptions * options) = 0;\nvirtual core::ComponentOptions * get_options() = 0;\nvirtual void shutdown() = 0;"


#
# @brief Generate and handle the command line parsing.
class Option():

    #
    # @brief Parse the command line arguments.
    #
    # Parses the command line arguments, using the python argparser.
    def parse(self):
        prog = 'C++Skeleton creator'

        description = '''This application creates a dummy implementation for SIOX modules which will output their arguments using cout or store the information in an accessible array.'''

        argParser = argparse.ArgumentParser(description=description, prog=prog)

        argParser.add_argument('--output', '-o', action='store', nargs=1,
                               dest='outputFile', help='C++ filename to write the generated wrapper.')


        argParser.add_argument('--style', '-s',
                               action='store', default='cout', dest='style',
                               choices=['cout', 'list'],
                               help='''Choose which output-style to use.''')

        argParser.add_argument('inputFile', default=None,
                               help='Source header file to parse.')

        argParser.add_argument('--debug', '-d',
                               action='store_true', dest='debug',
                               help='Debug every line')

        args = argParser.parse_args()

        if args.outputFile:
            args.outputFile = args.outputFile[0]
	else:
	    args.outputFile = re.sub(r'[.][hpp]+', ".cpp", args.inputFile)
	
        return args
class Function():

    def __init__(self):

        # Return type of the function (int*, char, etc.)
        self.type = ''
        # Name of the function
        self.name = ''
        # A list of Parameters, a parameter is a extra class for storing.
        self.parameterList = []
        # A list of templates associated with the function.
        self.definition = ''
        self.usedTemplateList = []
        # Indicates that the function is the first called function of the
        # library and initialize SIOX.
        self.init = False
        # Indicates that the function is the last called function of the
        # library.
        self.final = False

    #
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

    #
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

    #
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


#
# @brief One parameter of a function.
#
# This class holds only data.
class Parameter():

    def __init__(self):
        # The type of the parameter.
        self.type = ''
        # The name of the parameter.
        self.name = ''

#
# @brief Parses a header file and looks for function definitions.
#
# The function parser searches through a header file using regular expressions.
class FunctionParser():

    #
    # @brief Constructor for the FunctionParser class.
    #
    # @param self The reference to this object.
    #
    # @param options An Option object.
    def __init__(self, options):

        self.inputFile = options.inputFile
	self.debug = options.debug        

        self.regexClass = re.compile( 'class[\s]+(\w+)[\s]*:' , re.S
        );
        self.regexNamespace = re.compile( "namespace[\s]+(\w+)[\s]*{")

        # This regular expression searches for the general function definition.
        self.regexFunctionDefinition = re.compile(
            'virtual[\s]+([^(]+)[\s]+(\w+)[\s]*[(]([^);]*)[)][\s]*=[\s]*0[\s]*;', re.S)
        
        self.regexSIOXInterface = re.compile( "#define\s+(\w+_INTERFACE)\s+\"(\w+)\"")

        # This regular expression matches parameter type and name.
        self.regexParamterDefinition = re.compile('(.*)[\s]+(\w+)[\s]*')

        # This tuple of filter words searches for reseverd words in the
        # function return type.
        # Some typedefs can look like function a definition the regex.
        self.filter = ('typedef', '//', '#', 'return')

    #
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

	className = ""
	for curClass in self.regexClass.finditer(string):
	  if self.debug: 
		print ("Found class: " + curClass.group(1))
	  if className == "":
	    className = curClass.group(1)
	
	if className == "":
	   print("Error no class definition found in header")
	   return

	namespace = []
	for curNamespace in self.regexNamespace.finditer(string):
	  if self.debug: 
	  	print ("namespace: " + curNamespace.group(1))
	  namespace.append( curNamespace.group(1))
	  
	interfaceNameGroup = self.regexSIOXInterface.search(string)
	if interfaceNameGroup == None:
	   print("Error no INTERFACE definition found in header")
	   return
	interfaceName = interfaceNameGroup.group(2)
	if self.debug: 	
		print("Interface: " + interfaceName)

        # Match all function definitions
        iterateFunctionDefinition = self.regexFunctionDefinition.finditer(
            string)

        for functionDefinition in iterateFunctionDefinition:
            
            function = Function()

            # Extract the type an name form the match object.
            function.type = functionDefinition.group(1).strip()
            # Adjust return type
            function.typeBasic = function.type
            if function.typeBasic.find("const") == 0:
	      function.typeBasic = function.typeBasic[6:]
            function.typeBasic = function.typeBasic.replace("&", "")
            
            function.name = functionDefinition.group(2).strip()
	    # The parameter string.
            parameterString = functionDefinition.group(3).strip()
            if self.debug: 
            	print("Member: " + function.name + " with " + parameterString);
            
            # Substitute multiple blanks
            parameterString = re.sub('\s+', ' ', parameterString)

            # If the C source code should be generated split the string into a
            # list of parameters and extract the type and name of the
            # parameter.
	    parameterList = parameterString.strip().split(',')
	    function.parameterString = parameterString

	    for parameter in parameterList:
		parameterObject = Parameter()
		parameter = parameter.strip()

		if parameter in ('void', '...', ''):
		    parameterName = ''
		    parameterType = parameter
		    continue
		else:
		    parameterMatch = self.regexParamterDefinition.match(parameter)
		    parameterType = parameterMatch.group(1)
		    parameterName = parameterMatch.group(2)

		parameterObject.name = parameterName
		parameterObject.type = parameterType
		function.parameterList.append(parameterObject)

            functionList.append(function)
        return [className, namespace, interfaceName, functionList]

    #
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
        string = fh.read() + componentFunctions
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


#
# @brief The output class (write a file to disk)
#
class Writer():

    #
    # @brief The constructor
    #
    # @param options The supplied arguments
    def __init__(self, options):
        self.outputFile = options.outputFile
        self.inputFile = options.inputFile


    def sourceFileOut(self, parsedList, style):
        # open the output file for writing
        
        (className, namespace, interfaceName, functionList) = parsedList
        
        output = open(self.outputFile, 'w')
        print("#include <" + self.inputFile  + ">", file=output)
        print("#include <iostream>\n\nusing namespace std;", file=output)
        
        for names in namespace:
	   print("using namespace " + names + ";", file=output)
        
        newClassName = className + "_" + style
        
        print("\nclass " + newClassName + " : " + className + "{" , file=output)
        # implement all functions:
        for function in functionList:
	  print("  " + function.type + " " + function.name + "(" + function.parameterString + "){", file=output)
	  if style == "cout":
	      print("    cout << \"" + className + " - " + function.name + "(\";", file=output)
	      for parameter in function.parameterList:
	        print("    cout << " + parameter.name + ' << ", ";', file=output)
              print('    cout << ")" << endl ;', file=output)

	  if (function.type != "void"):
	    print("    " + function.typeBasic + " var;", file=output)	    
	    print("    return var;\n", file=output)

	  print("  }", file=output)
        
        print("};\n" , file=output)
        
        print('extern "C"{\nvoid * get_instance_' + interfaceName + '() {\n return new ' + newClassName + '(); \n}\n}', file = output)
	output.close()


def main():
    opt = Option()
    options = opt.parse()

    functionParser = FunctionParser(options)
    outputWriter = Writer(options)

    functions = functionParser.parseFile()
    outputWriter.sourceFileOut(functions, options.style)

        
if __name__ == '__main__':
    main()
