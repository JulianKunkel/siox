#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse
import re
import subprocess

# from wrapper_conf import (before, beforeTracing, after, attributes, conditions,
# Options)

# the signatures of the functios to instrument
functions = []
# the path to store the output files
output = ['./out.c']
# print out debug informations if True
debug = None
def init():
	"""Initialize the command line parser. Returns a parser from argparse."""

	# FIXME: sort the options
	argParser = argparse.ArgumentParser(description='''Wraps SIOX function
			around MPI function calls''')

	argParser.add_argument('--output', '-o', action='store', nargs=1,
			dest='output', help='out')
	

	argParser.add_argument('--debug', '-d', action='store_true', default=False,
			dest='debug', help='''Print out debug information.''')

	argParser.add_argument('header',metavar='files',action='store', nargs='+',
			help='The header file to instrument.')
	
	return argParser

def parse_header():
	

	cppProc = subprocess.Popen(['cpp',headerFile[0]], stdin=subprocess.PIPE,
			stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	
	header, error = cppProc.communicate()

	for line in header:

		# ^\s* -> leading white space
		# (\w+)\s* -> the return value
		# (\*)?\s* -> matches a * if the return value is a pointer don't have to
		#		appear 
		# (\w+)\s* -> matches the function name
		# \( -> matches opening parentheses for the signature
		# (.+) -> matches the parameters of the function 
		# \) -> matches the closing parentheses for the signature
		# \s*;\s*$ -> matches the colon and the trailing white space

		regex = re.compile("^\s*(\w+)(\s*(\*)\s*)|(\s+)(\w+)\s*\((.+)\)\s*;\s*",
				re.M)

		regex = regex.match(line)

		if not regex:
			continue
		
		returnValue = regex.group(3)

		if regex.group(4):
			returnPointer = True
		else:
			returnPointer = False

		functionName = regex.group(5)
		signature = regex.group(6)
		
		if debug:
			print "Return Value: %s" % returnValue
			print "Pointer? %s" % returnPointer
			print "Function Name: %s" % functionName
			print "Function Signature: %s\n\n" % signature
		
		functions.append([])
		function = functions[-1]

		# ReturnValue = function[0]
		function.append(returnValue)
		# ReturnPointer = function[1]
		function.append(returnPointer)
		# FunctionName = function[2]
		function.append(functionName)
		# Signature = function[3]
		function.append(signature)

#
# start of the main program
#

# initialize the command line argument parser 
argParser = init()

# read the command line arguments
args = argParser.parse_args(sys.argv[1:])

# get the parsed arguments
headerFile = args.header

if args.output is not []:
	output = args.output 

output = output[0]
debug = args.debug


parse_header()

# open the output file for writing
file = open(output, "w")

# Function headers
for function in functions:
	file.write("static ")
	file.write(function[0]+" ")
	
	if function[1]:
		file.write("* ")
		
	file.write("(* static_")
	file.write(function[2]+") ( ")
	file.write(function[3]+" ) = NULL;\n")

# Function definitions
for function in functions:
	file.write(function[0] + " " + function[2] + "(" + function[3] + ") {\n");
	file.write(function[0] + " ret = (* static_" + function[2] + ") ();\n");
	file.write("return ret;\n");
	file.write("}\n");

# Generic needs
file.write("""
#define OPEN_DLL(defaultfile, libname) 
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
	file.write("\nADD_SYMBOL("+function[2]+");\n")
	file.write("static_"+function[2]+" = symbol;")
