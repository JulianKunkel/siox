#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse
from pycparser import c_parser, c_ast, parse_file

# from wrapper_conf import (before, beforeTracing, after, attributes, conditions,
# Options)

DEBUG = False

		

class FuncDefVisitor(c_ast.NodeVisitor):
	
	def __init__(self):

		self.functions = [] 
		self.storage = None

	def visit_FuncDef(self, node):
		
		# We found a function definition! \o/
		self.storage = Function()
		self.functions.append(self.storage)
		
		
		if DEBUG:
			print('Function %s at %s' % (node.decl.name, node.decl.coord))
		
		function = self.functions[-1]
		function.name = node.decl.name
		
		self.getType(node.decl.type.type)

		# Get the parameter

		if node.decl.type.args:
			params = node.decl.type.args.params
			
			# Iterate over the parameters
			for param in params:

				# Append a new parameter
				self.storage = Parameter()
				function = self.functions[-1]
				function.parameters.append(self.storage)
				
				self.storage.name += param.name
				# Recursive run through the abstract syntax tree of the parameter
				self.getType(param)
		

	def getType(self, decl):

		typ = type(decl)
		
		if typ == c_ast.TypeDecl:
			self.getType(decl.type)
			

		elif typ == c_ast.Typename or typ == c_ast.Decl:
			self.getType(decl.type)
		
		# I found Waldo! After several hours!
		# Stores the  type of the parameter.
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
		
		# Add * to the parameter type if it's a array
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

def writeOutputFile(options, functions):
	# open the output file for writing
	file = open(options.outputFile, 'w')

	# generate signatures
	for function in functions:
		signature = '';
	
		for sigpart in function.parameters:						
			sig = sigpart.type + '' + sigpart.name + ', '
			function.signature += sig
		function.signature = function.signature[:-2]

	# only generate a header-skeleton for the user to comment
	if options.blankHeader:
		# Function headers
		for function in functions:
			file.write(function.type+" ")
			file.write(function.name+" ( ")
			file.write(function.signature+" ); \n")	

	# use the header-skeleton commented by the user to generate the instrumented library
	else:
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
		file.write("""#define OPEN_DLL(defaultfile, libname) 
{ 
	char * file = getenv(libname); 
	if (file == NULL)
		file = defaultfile;
	dllFile = dlopen(file, RTLD_LAZY); 
	if (dllFile == NULL) { 
		printf("[Error] dll not found %s", file); 
		exit(1); 
	} 
}

#define ADD_SYMBOL(name) 
symbol = dlsym(dllFile, #name);
if (symbol == NULL) {
	printf("[Error] trace wrapper - symbol not found %s", #name); 
}
""")

		# Symbols
		for function in functions:
			file.write("\nADD_SYMBOL("+function.name+");\n")
			file.write("static_"+function.name+" = symbol;")

		file.close()

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

	writeOutputFile(opt, functionDefs.functions)

main()
