#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse
from pycparser import c_parser, c_ast, parse_file


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
		if node.children()[0].__class__ != c_ast.FuncDecl:
			return	

		# We found a function definition! \o/
		# Create a new function and append it to the list of found functions
		# The self.storge variable is used to unifiy the accsess 
		function = Function()
		self.functions.append(self.storage)
		
		storage.name = node.name

		if DEBUG:
			print('Function %s' % (self.storage.name))
		
		
		self.getType(node.type.type, storage)

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
			self.getType(decl.type)
			

		elif typ == c_ast.Typename or typ == c_ast.Decl:
			self.getType(decl.type)
		
		# I found Waldo! After several hours!
		# Stores the type of the parameter.
		elif typ == c_ast.IdentifierType:
			identifier= ''
			
			# Join the identifiers like 'unsigned int' into one string
			for name in decl.names:
				identifier += name + ' '

			if DEBUG:
				print('Found identifier: %s' % (identifier))
			
			self.storage.type = identifier

		# Add a * to the parameter type if there is a pointer
		elif typ == c_ast.PtrDecl:
			self.getType(decl.type)

			self.storage.type += '*'
		
		# Add * to the parameter type if it's a array like foo[] 
		elif typ == c_ast.ArrayDecl:
			self.getType(decl.type)

			self.storage.type += '*'

class Function():

	def __init__(self):
		"""docstring for __init__"""
		self.type= ''
		self.name = ''
		self.parameters = []
		self.signature = ''

	def identyfier(self):
		return self.name+self.signature

class Parameter():

	def __init__(self):
		"""docstring for __init__"""
		self.type = ''
		self.name = ''

#def run():
	#pass

def Option():

	# FIXME: sort the options
	argParser = argparse.ArgumentParser(description='''Wraps SIOX function
			around MPI function calls''')

	argParser.add_argument('--output', '-o', action='store', nargs=1,
			dest='outputFile', default="out.c", help='out')
	
	argParser.add_argument('--debug', '-d', action='store_true', default=False,
			dest='debug', help='''Print out debug information.''')

	argParser.add_argument('--source', '-s', action='store', nargs=1,
			dest='sourceFile', help='A C source code file that should be parsed.')	

	argParser.add_argument('--header', '-g', action='store', nargs=1,
			dest='headerFile', help='''A C header file that will be parsed''')

	argParser.add_argument('--blankHeader', '-b', action='store_true', default=False,
			dest='blankHeader', help='''Only generate a clean header file''')
	
	args = argParser.parse_args()

	if args.headerFile:
		args.headerFile = args.headerFile[0]

	if args.sourceFile:
		args.sourceFile = args.sourceFile[0]

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

	# Function headers
	for function in functions:
		file.write(function.type+" ")
		file.write(function.name+" ( ")
		file.write(function.signature+" ); \n")	

	# close the file
	file.close()

def writeSourceFile(options, functions):
	# open the output file for writing	
	file = open(options.outputFile, 'w')
	
	# Function headers
	for function in functions:
		file.write("static ")
		file.write(function.type+" ")
		file.write(" (* static_")
		file.write(function.name+") ( ")
		file.write(function.signature+" ) = NULL;\n")
		
	file.write("\n")

	# Function definitions
	for function in functions:
		file.write(function.type + " " + function.name + "(" + function.signature + ") {\n")
		file.write("	" + function.type + " ret = (* static_" + function.name + ") (" + function.signature + ");\n")
		file.write("	return ret;\n")
		file.write("}\n\n")

	# Generic needs
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
		file.write("\nADD_SYMBOL("+function.name+");\n")
		file.write("static_"+function.name+" = symbol;")

	# close the file
	file.close()

def writeOutputFile(options, functions):
	
	# Generate all function-signatures for writing
	generateSignatures(functions)

	if options.blankHeader:
		# only generate a header-skeleton for the user to comment
		writeHeaderFile(options, functions)

	else:
		# use the header-skeleton commented by the user to generate the instrumented library
		writeSourceFile(options, functions)

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
	
	if not (opt.headerFile or opt.sourceFile):
		print('No source file or header file provided. Nothing to compute.')
		sys.exit(1)
	
	if opt.headerFile:
		if DEBUG:
			print('DEBUG: Parsing header file: %s' % (opt.headerFile))

		headerAST = parse_file(opt.headerFile, use_cpp=True)
		functionDefs.visit(headerAST)
	
	if opt.sourceFile:
		if DEBUG:
			print('DEBUG: Parsing source code file: %s' % (opt.sourceFile))

		sourceAST = parse_file(opt.sourceFile, use_cpp=True)

		functionDefs.visit(sourceAST)	

main()
