#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse
from pycparser import c_parser, c_ast, parse_file
from collections import OrderedDict

# import the template
from template import *

DEBUG = False

class FuncDefVisitor(c_ast.NodeVisitor):
	
	def __init__(self):

		self.functions = [] 

	def visit_Decl(self, node):
		
		# If the found declaration has a not a function declaration
		# jump out.
		# This is necessary because in a header file int foobar(void); is not
		# recognised as a function definition by the pycparser and the function
		# declaration is the function name without the return type.
		# So we search for a type declaration and check if the next decalration is
		# a function. Sounds strange but it works.
		if type(node.children()[0])!= c_ast.FuncDecl:
			return	

		# We found a function definition! \o/
		# Create a new function and append it to the list of found functions
		# The self.storge variable is used to unifiy the accsess 
		function = Function()
		self.functions.append(function)

		function.name = node.name

		if DEBUG:
			print('Function %s' % (function.name))
		
		
		self.getType(node.type.type, function)

		# Get the parameter
		if node.type.args:
			params = node.type.args.params
			
			# Iterate over the parameters and get the name an type
			for param in params:
				
				# Append a new parameter
				parameter = Parameter()
				function = self.functions[-1]
				function.parameters.append(parameter)
				
				# Get the name of the parameter and parse the type
				parameter.name += param.name
				self.getType(param, parameter)
	
	##
	# @brief Recursive run through the type definitions of the function or 
	# parameter
	#
	# 
	# This is needed because every pointer is a new node in the declaration,
	# ** are two nodes in depth.
	#
	# @param decl The current node in the declaration
	# @param storage A reference to the object to store the parameter type,
	# this could be the return type of a function or a type of function parameter
	#
	# @return Nothing 
	def getType(self, decl, storage):
		
		typ = type(decl)
		
		if typ == c_ast.TypeDecl:
			self.getType(decl.type, storage)
			

		elif typ == c_ast.Typename or typ == c_ast.Decl:
			self.getType(decl.type, storage)
		
		# I found Waldo! After several hours!
		# Stores the type of the parameter.
		elif typ == c_ast.IdentifierType:
			identifier= ''
			
			# Join the identifiers like 'unsigned int' into one string
			for name in decl.names:
				identifier += name + ' '

			if DEBUG:
				print('Found identifier: %s' % (identifier))
			
			storage.type = identifier

		# Add a * to the parameter type if there is a pointer
		elif typ == c_ast.PtrDecl:
			self.getType(decl.type, storage)

			storage.type += '*'
		
		# Add * to the parameter type if it's a array like foo[] 
		elif typ == c_ast.ArrayDecl:
			self.getType(decl.type, storage)

			storage.type += '*'


class InstructionParser():

	def parse(self, functions, inputFile):

		inputFileHandel = open(inputFile, 'r')

		inputLines = inputFileHandel.readlines()
		commands = template.keys()
		
		instructions = OrderedDict()
		
		functionCounter = 0

		for function in functions:
			instructions[function.getIdentyfier()] = InstrumentedFunction()
		
		identyfier, sections = instructions.popitem()
		storage = None
		for index, line in enumerate(inputLines):
			
			line = line.lstrip('/ \t')
			line = line.rstrip()

			wordList = line.split()
			
			key = ''.join(wordList)
			
			if identyfier == key:

				instructions[identyfier] = sections 
				
				identyfier, sections = instructions.popitem()
				storage = None

			else:

				for word in wordList:

					if word == 'before':
						storage = sections.before

					elif word == 'after':
						storage = sections.after

					elif word == 'init':
						storage = sections.init

					elif word in commands:
						
						if storage == None:
							print('ERROR: No section defined for %s at %i, please use "before", "after" or "init"' % (word, index))
							sys.exit(1)

						instruction = Instruction()
						instruction.name = word
						storage.append(instruction)

					else:

						if instruction.name == '':
							print('ERROR: Template name not found! %s is not a valid instrumention instruction in line %i.' % (word, index))
							sys.exit(1)

						storage[-1].parameters.append(word)

		instructions[identyfier] = sections
		return instructions	

class Function():

	def __init__(self):
		"""docstring for __init__"""
		self.type= ''
		self.name = ''
		self.parameters = []
		self.signature = ''

	def getIdentyfier(self):
		
		params = ''	

		for param in self.parameters:
			params += '%s,' % (param.__str__())
		
		params = params[:-1]

		identyfier = '%s%s(%s);' % (''.join(self.type.split(' ')),
			self.name, params)

		return identyfier

class Parameter():

	def __init__(self):
		"""docstring for __init__"""
		self.type = ''
		self.name = ''

	
	def __str__(self):
		
		return ''.join(self.type.split(' '))+self.name


class InstrumentedFunction():

	def __init__(self):

		self.init = []
		self.before = []
		self.after = []

class Instruction():
	
	def __init__(self):
		self.name = ''
		self.parameters = []


def Option():

	# FIXME: sort the options
	argParser = argparse.ArgumentParser(description='''Wraps SIOX function
			around MPI function calls''')

	argParser.add_argument('--output', '-o', action='store', nargs=1,
			dest='outputFile', default="out.c", help='out')
	
	argParser.add_argument('--debug', '-d', action='store_true', default=False,
			dest='debug', help='''Print out debug information.''')

	argParser.add_argument('--blankHeader', '-b', action='store_true', default=False,
			dest='blankHeader', help='''Only generate a clean header file''')
	
	argParser.add_argument('inputFile', default=None, 
	help='Source or header file to parse')

	args = argParser.parse_args()

	if args.outputFile:
		args.outputFile = args.outputFile[0]

	return args

def generateSignatures(functions):
	for function in functions:
		signature = '';
	
		for sigpart in function.parameters:						
			sig = sigpart.type + '' + sigpart.name + ', '
			function.signature += sig
		function.signature = function.signature[:-2]

def writeHeaderFile(options, functions):
	# open the output file for writing
	file = open(options.outputFile, 'w')

	# function headers
	for function in functions:
		file.write("%s %s ( %s ); \n" % (function.type, function.name, function.signature))

	# close the file
	file.close()

def writeSourceFile(options, functions, instructions):

	# open the output file for writing	
	file = open(options.outputFile, 'w')
	
	# includes
	for i in includes:
		file.write(i+"\n")
	file.write("\n")

 	# global variables
	for i in variables:
		file.write(i+"\n")
	file.write("\n")

	# function headers
	for function in functions:
		file.write("static %s  (* static_%s) ( %s ) = NULL;\n"  % (function.type, function.name, function.signature))
	file.write("\n")

	# function definitions
	for function in functions:
		file.write("%s %s(%s) {\n" % (function.type, function.name, function.signature))

#		if instructions.has_key(function.getIdentyfier()):
#			for i in instructions[function.getIdentyfier()]:
#				if template[i[0]]["before"] != "":
#					file.write("%s \n" % (template[i[0]]["before"] % tuple(i[1])))

		if instructions.has_key(function.getIdentyfier()):

			lines = instructions[function.getIdentyfier()]
			for line in lines.before:
				if template[line.name] != "":
					file.write("%s \n" % (template[line.name]["before"] % tuple(line.parameters)))

		file.write("%s ret = (* static_%s) (%s);\n" % (function.type, function.name, function.signature))

#		if instructions.has_key(function.getIdentyfier()):
#			for i in instructions[function.getIdentyfier()]:
#				if template[i[0]]["after"] != "":
#					file.write("%s \n" % (template[i[0]]["after"] % tuple(i[1])))

		if instructions.has_key(function.getIdentyfier()):
			lines = instructions[function.getIdentyfier()]
			for line in lines.after:
				if template[line.name] != "":
					file.write("%s \n" % (template[line.name]["after"] % tuple(line.parameters)))

		file.write("return ret;\n}\n\n")

	# generic needs
	file.write("""#define OPEN_DLL(defaultfile, libname) \\
{ \\
	char * file = getenv(libname); \\
	if (file == NULL) \\
		file = defaultfile; \\
	dllFile = dlopen(file, RTLD_LAZY); \\
	if (dllFile == NULL) { \\
		printf("[Error] dll not found %s", file); \\
		exit(1); \\
	} \\
}

#define ADD_SYMBOL(name) \\
symbol = dlsym(dllFile, #name); \\
if (symbol == NULL) { \\
	printf("[Error] trace wrapper - symbol not found %s", #name); \\
}
""")

	# Symbols
	for function in functions:
		file.write("\nADD_SYMBOL(%s);\nstatic_%s = symbol;" % (function.name, function.name))

	# close the file
	file.close()

def writeOutputFile(options, functions, instructions):
	
	# Generate all function-signatures for writing
	generateSignatures(functions)

	if options.blankHeader:
		# only generate a header-skeleton for the user to comment
		writeHeaderFile(options, functions)

	else:
		# use the header-skeleton commented by the user to generate the instrumented library
		writeSourceFile(options, functions, instructions)

#
# start of the main program
#
def main():
	# parse the options
	opt = Option()
	DEBUG = opt.debug

	functionDefs = FuncDefVisitor()

	# abstract syntax tree for the header- and the source code file 
	headerAST = None
	sourceAST = None 
	
	if DEBUG:
		print('DEBUG: Parsing file: %s' % (opt.inputFile))

	sourceAST = parse_file(opt.inputFile, use_cpp=True)

	functionDefs.visit(sourceAST)	
	functions = functionDefs.functions

	instructions = InstructionParser()
	instr = instructions.parse(functions, opt.inputFile)

	# TODO: foobar ersetzten	
	writeOutputFile(opt, functions, instr)

main()
