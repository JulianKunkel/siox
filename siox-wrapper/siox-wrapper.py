#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8
from __future__ import print_function

import re
import sys
import argparse
import subprocess
# import the template
from template import *

DEBUG = False



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

        argParser.add_argument('--debug', '-d', action='store_true', default=False,
        dest='debug', help='''Print out debug information.''')

        argParser.add_argument('--blank-header', '-b', action='store_true',
                default=False, dest='blankHeader',
                help='''Generate a clean header file wich can be instrumented
from a other header file or C source file.''')

        argParser.add_argument('--cpp', '-c', action='store_true', default=False,
        dest='cpp', help='Use cpp to pre process the input file.')

        argParser.add_argument('--cpp-args', '-a', action='store', nargs=1, default=[],
        dest='cppArgs', help='''Pass arguments to the cpp. If this option is
specified the --cpp option is specified implied.''')

        argParser.add_argument('--alt-var-names', '-n',
        action='store_true', default=False, dest='alternativeVarNames',
        help='''If a header file dose not provide variable names, this will
enumerate them.''')

        argParser.add_argument('--style', '-s',
        action='store', default='wrap', dest='style',
        choices=['wrap', 'dllsym'],
        help='''Determinates which output-style to use.''')

        argParser.add_argument('inputFile', default=None,
        help='Source or header file to parse.')

        args = argParser.parse_args()

        if args.outputFile:
            args.outputFile = args.outputFile[0]
        if args.cppArgs:
            args.cpp = True

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
        self.parameters = []
        ## A list of templates associated with the function.
        self.definition = ''
        self.usedTemplates = []
        ## Indicates that the function is the first called function of the library and initialize SIOX.
        self.init = False
        ## Indicates that the function is the last called function of the library.
        self.final = False

    ##
    # @brief Generate the function call.
    #
    # @param self The reference to this object.
    #
    # @return A string containing the function call.
    #
    # Generates the function call of the function.
    def getCall(self):

        if len(self.parameters) == 1:
            if self.parameters[0].name == 'void':
                return '%s()' % (self.name)
        return '%s(%s)' % (self.name, ', '.join(paramName.name for paramName in self.parameters))

    ##
    # @brief Generate the function definition.
    #
    # @param self The reference to this object.
    #
    # @return The function definition as a string without the type.
    def getDefinition(self):

        if self.definition == '':

            return '%s(%s)' % (self.name,
            ', '.join('  '.join([param.type, param.name]) for param in self.parameters))

        else:
            return '%s %s %s' % (self.type, self.name, self.definition)

    ##
    # @brief Generate an identifier of the function.
    #
    # The generated identifier is used as a key for the hash table the functions
    # are stored in.
    #
    # @return A unique identifier for the function as a string.
    def getIdentifier(self):

        identifier = '%s%s(%s);' % (self.type, self.name,
                ''.join(''.join([param.type, param.name]) for param in self.parameters))

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

    def __init__(self):
        ## Name of the instruction.
        self.name = ''
        ## Parameters of the instruction.
        self.parameters = []


##
# @brief Parses a header file and looks for function definitions.
#
# The function parser searches through a header file using regular expressions.
class FunctionParser():

    def __init__(self, options):

        self.inputFile = options.inputFile
        self.cpp = options.cpp
        self.cppArgs = options.cppArgs
        self.blankHeader = options.blankHeader
        # This regex searches from the beginning of the file or } or ; to the next
        # ; or {.
        ## This regular expression searches for the general function definition.
        self.regexFuncDef = re.compile('(?:;|})?(.+?)(?:;|{)', re.M | re.S)
        # Filter lines with comments beginng with / or # and key word that looks
        # like function definitions
        ## This list regular expression is used to filter the found definitions,
        ## because something could look like a definition but relay isn't.
        self.regexFilterList = [re.compile('^\s*[#/].*'),
                re.compile('^\s*typedef.*'),
                re.compile('^.*=.*')]

        ## This regular expression splits every line that is left in function name and parameters.
        self.regexFuncDefParts = re.compile('([a-zA-Z0-9_\-\s]+?)\((.+?)\).*')

    ##
    # @brief This function parses the header file.
    #
    # @return A list of function definitions.
    #
    # @param self The reference to the object.
    def parse(self, string):

        functions = []

        matchFuncDef = self.regexFuncDef.findall(string)

        for funcNo, function in enumerate(matchFuncDef):

            # The find all was a multi line match, one function definition
            # could be in multiple lines.
            function = function.split('\n')

            for index in range(0, len(function)):
                function[index] = function[index].strip()

                # Run the filter regex list.
                for regexFilter in self.regexFilterList:
                    if regexFilter.match(function[index]):
                        function[index] = ''

            # Rejoin the split function into a single line.
            function = ' '.join(function)

            funcParts = self.regexFuncDefParts.match(function)

            if funcParts is None:
                continue

            # At this point we are quite certain, we found a function definition
            function = Function()
            functions.append(function)

            if self.blankHeader:
                # Get type and the name of the function.
                function.type, function.name = self.getTypeName(funcParts.group(1).strip())
                function.definition = "(" + funcParts.group(2) + " )"

            else:

                function.type, function.name = self.getTypeName(funcParts.group(1))
                parameters = funcParts.group(2).split(',')

                # Get the type and name of the parameters.
                for paramNo, parameter in enumerate(parameters):
                    parameterObj = Parameter()
                    function.parameters.append(parameterObj)
                    parameterObj.type, parameterObj.name = self.getTypeName(parameter)

        return functions

    # @brief Wrapper function for parse().
    #
    # @return A list of function objects
    #
    # @param self
    #
    # This is a wrapper function for the parse() function for parsing the
    #passed to the FunctionParser object with the option argument.
    def parseFile(self):

        # Call the cpp.
        if self.cpp:
            cppCommand = ['cpp']
            cppCommand.extend(self.cppArgs)
            cppCommand.append(self.inputFile)
            cpp = subprocess.Popen(cppCommand,
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            string, error = cpp.communicate()

            if error != '':
                print('ERROR: CPP error:\n', error, file=sys.stderr)
                sys.exit(1)

        else:
            inputFile = open(self.inputFile, 'r')
            string = inputFile.read()

        functions = self.parse(string)
        return functions

    # @brief Wrapper function for parse()
    #
    # @return A list of function objects
    #
    # @param self
    #
    # param string
    def parseString(self, string):

        functions = self.parse(string)
        return functions

    ##
    # @brief Get the type and the name of a declaration.
    #
    # @param declaration A declaration of a function or parameter.
    # @param alternativeVarName If the header file doesn't provide variable
    # names they will be prefixed and enumerated.
    #
    # @return The type and the name of the declaration as a tuple.
    def getTypeName(self, declaration):

        type = ''
        name = ''
        arrayPointer = ''
        # this is needed for converting array[] to array*
        regexBrackets = re.compile('\[\s*\]')

        if regexBrackets.search(declaration):
            declaration = regexBrackets.sub('', declaration)
            arrayPointer = '*'

        parameterParts = declaration.split('*')

        if(len(parameterParts) == 1):
            parameterParts = declaration.split()
            # Pop the name of the declaration.
            name = parameterParts.pop().strip()

            for element in parameterParts:
                element = element.strip()
                type += ' %s' % (element)
            type += arrayPointer

        else:

            arrayPointer += '*'
            name = parameterParts.pop()

            nameParts = name.split()

            if len(nameParts) == 1:
                name = nameParts.pop().strip()

            elif len(nameParts) > 1:
                name = nameParts.pop().strip()

            for element in parameterParts:
                element = element.strip()
                if element is '':
                    type += '*'

                else:
                    type += '  %s' % (element)

                type += arrayPointer
                arrayPointer = ''

            for element in nameParts:
                type += ' %s' % (element)

        return (type.strip(), name)


##
# @brief The command parser which reads the instrumentation instructions.
#
# The command parser reads the instrumentation form a commented header file.
# The parsing is based on regular expressions.
class CommandParser():

    def __init__(self, options):
        self.inputFile = options.inputFile
        self.options = options
        ## This regular expression matches the instructions which begin with //
        self.commandRegex = re.compile('^\s*//\s*(.+?)\s+(.*)')

    ##
    # @brief This function parses the header file.
    #
    # @param self The reference to the object.
    # @param functions The list of function objects returned by the function
    # parser.
    #
    #
    # @return A list of function objects which are extended by the
    # instrumentation instructions.
    def parse(self, functions):

        functionParser = FunctionParser(self.options)

        avalibalCommands = template.keys()
        input = open(self.inputFile, 'r')
        inputLines = input.readlines()
        index = 0
        commandName = ''
        commandArgs = ''
        templateList = []
        currentFunction = ''

        # Iterate over every line and search for instrumentation instructions.
        for line in inputLines:

            currentFunction = functionParser.parseString(line)
            if len(currentFunction) == 1:
                currentFunction = currentFunction[0]
            else:
                currentFunction = Function()

            match = self.commandRegex.match(line)
            if (match):
                # Search for init or final sections to define init or final
                # functions
                if match.group(1) == 'init':
                    functions[index].init = True

                elif match.group(1) == 'final':
                    functions[index].final = True

                elif match.group(1) in avalibalCommands:
                    # If a new command name is found and a old one is still
                    # used, save the old command and begin a new instruction
                    if commandName != '':
                        templateList.append(templateClass(commandName, commandArgs))
                        commandName = ''
                        commandArgs = ''


                    commandName += match.group(1)
                    commandArgs += match.group(2)

                else:
                    # If no command name is defined and a comment which is no
                    # instruction is found throw an error.
                    if commandName == '':
                        print("ERROR: Command not known: ", match.group(1), file=sys.stderr)
                        sys.exit(1)

                    commandArgs = match.group(1)
                    commandArgs = match.group(2)
            else:
                #If a function is found append the found instructions to the function object.
                if currentFunction.getIdentifier() == functions[index].getIdentifier():
                    if commandName != '':
                        templateList.append(templateClass(commandName, commandArgs))
                        functions[index].usedTemplates = templateList
                        templateList = []
                        commandName = ''
                        commandArgs = ''
                    index = index + 1

        return functions


##
# @brief Used to store the templates for each funtion
class templateClass():
    ##
    # @brief The constructor
    #
    # @param name The name of the used template
    # @param variables The used variables
    def __init__(self, name, variables):
        templateDict = template[name]
        self.name = name
        self.parameters = {}

        # Generate strings for output from given input
        self.setParameters(templateDict['variables'], variables)

        # Remember template-acces for easier usage
        self.world = templateDict['global']
        self.init = templateDict['init']
        self.before = templateDict['before']
        self.after = templateDict['after']
        self.final = templateDict['final']

    ##
    # @brief Reads the parameteres and generates the needed output
    #
    # @param names The name of the used template
    # @param values The used variables
    def setParameters(self, names, values):
        # generate a list of all names and values
        NameList = names.split(' ')
        ValueList = values.split(' ')

        position = 0
        # iterate over all elements but the last one
        for value in ValueList[0:len(NameList) - 1]:
            self.parameters[NameList[position]] = value
            position += 1
        # all other elements belong to the last parameter
        self.parameters[NameList[position]] = ' '.join(ValueList[len(NameList) - 1:])

    ##
    # @brief Used for selective output
    #
    # @param type What part of the template should be given
    #
    # @return The requested string from the template
    def output(self, type):
        if (type == 'global'):
            return self.world % self.parameters
        elif (type == 'init'):
            return self.init % self.parameters
        elif (type == 'before'):
            return self.before % self.parameters
        elif (type == 'after'):
            return self.after % self.parameters
        elif (type == 'final'):
            return self.final % self.parameters
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

    ##
    # @brief Write a header file
    #
    # @param functions A list of function-objects to write
    def headerFile(self, functions):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all function headers
        for function in functions:
            for match in throwaway:
                if re.search(match, function.getDefinition()):
                    function.name = '';

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
    def sourceFileWrap(self, functions):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all needed includes
        for match in includes:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)

        # write all global-Templates
        for func in functions:
            for templ in func.usedTemplates:
                if templ.output('global') != '':
                    print(templ.output('global'), file=output)
        print("", file=output)

        # write all function redefintions
        for function in functions:
            print(function.type, ' __real_', function.getDefinition(),
                    end=';\n', sep='', file=output)

        print("", file=output)

        # write all functions-bodies
        for function in functions:
            # write function signature

            print(function.type, ' __wrap_', function.getDefinition(),
                    end='\n{\n', sep='', file=output)

            # a variable to save the return-value
            returntype = function.type

            if returntype != "void":
                print('\t', returntype, ' ret;', end='\n', sep='',
                        file=output)

            # is this the desired init-function?
            if function.init:
                # write all init-templates
                for func in functions:
                    for temp in func.usedTemplates:
                        outstr = temp.output('init').strip()
                        if outstr.strip() != '':
                            print('\t', outstr, end='\n', sep='', file=output)

            # write the before-template for this function
            for temp in function.usedTemplates:
                outstr = temp.output('before').strip()
                if outstr != '':
                    print('\t', outstr, end='\n', sep='', file=output)

            # write the function call
            if returntype != "void":
                print('\tret = __real_', function.getCall(), end=';\n', sep='',
                        file=output)
            else:
                print('\t__real_', function.getCall(), end=';\n', sep='',
                        file=output)

            # is this the desired final-function?
            if function.final:
                # write all final-functions
                for func in functions:
                    for temp in func.usedTemplates:
                        outstr = temp.output('final').strip()
                        if outstr.strip() != '':
                            print('\t', outstr, end='\n', sep='', file=output)

            # write all after-templates for this function
            for temp in function.usedTemplates:
                outstr = temp.output('after').strip()
                if outstr != '':
                    print('\t', outstr, end='\n', sep='', file=output)

            # write the return statement and close the function
            if returntype != "void":
                print('\treturn ret;\n}', end='\n\n', file=output)

            else:
                print('\n}', end='\n\n', file=output)

        # generate gcc string for the user
        gcchelper = ''

        for func in functions:
            gcchelper = "%s,\"%s\"" % (gcchelper, func.name)

        print(gcchelper[1:])

        # close the file
        output.close()

    ##
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def sourceFileDLLSym(self, functions):
        # open the output file for writing
        output = open(self.outputFile, 'w')

        # write all global-Templates
        for temp in functions[0].usedTemplates:
            print(temp.output('global'), file=output)
        print("\n\n", file=output)

        # write the redefinition of all functions
        for function in functions:
            print('static ', function.type, ' (* static_', function.name,
             ') ( ', ", ".join((' '.join([param.type, param.name]) for param in function.parameters)), end=' ) = NULL;\n',
             sep='', file=output)

        # write all functions-bodies
        for function in functions:
            # write function signature
            print(function.type, function.getDefinition(),
                    end='\n{\n', sep='', file=output)

            # a variable to save the return-value
            returntype = function.type
            returntype = re.sub('^\s*extern\s*', '', returntype)
            print('\t', returntype, ' ret;', end='\n', sep='',
                    file=output)

            # is this the desired init-function?
            if function.init:
                # write all init-templates
                for func in functions:
                    for temp in func.usedTemplates:
                        outstr = temp.output('init').strip()
                        if outstr.strip() != '':
                            print('\t', outstr, end='\n', sep='', file=output)

            # write the before-template for this function
            for temp in function.usedTemplates:
                outstr = temp.output('before').strip()
                if outstr != '':
                    print('\t', outstr, end='\n', sep='', file=output)

            # write the function call
            print('\tret = (* static_', function.getCall(), end=');\n', sep='',
                    file=output)

            # is this the desired final-function?
            if function.final:
                # write all final-functions
                for func in functions:
                    for temp in func.usedTemplates:
                        outstr = temp.output('final').strip()
                        if outstr.strip() != '':
                            print('\t', outstr, end='\n', sep='', file=output)

            # write all after-templates for this function
            for temp in function.usedTemplates:
                outstr = temp.output('after').strip()
            # write the return statement and close the function
            print('\treturn ret;\n}', end='\n\n', file=output)

        # generic needs
        print("""#define OPEN_DLL(defaultfile, libname) \\
{ \\
  char * file = getenv(libname); \\
  if (file == NULL) \\
    file = defaultfile; \\
  dllFile = dlopen(file, RTLD_LAZY); \\
  if (dllFile == NULL) { \\
    printf("[Error] dll not found %s", file); \\
    exit(1); \\
  } \\
}            pass

#define ADD_SYMBOL(name) \\
symbol = dlsym(dllFile, #name); \\
if (symbol == NULL) { \\
  printf("[Error] trace wrapper - symbol not found %s", #name); \\
}
""", file=output)

        for function in functions:
            print("\nADD_SYMBOL(", function.name, ");\nstatic_", function.name, " = symbol;", file=output)

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

    functions = functionParser.parseFile()

    if options.blankHeader:
        outputWriter.headerFile(functions)

    else:
        commandParser = CommandParser(options)
        functions = commandParser.parse(functions)
        if options.style == "wrap":
            outputWriter.sourceFileWrap(functions)
        else:
            outputWriter.sourceFileDLLSym(functions)

if __name__ == '__main__':
    main()
