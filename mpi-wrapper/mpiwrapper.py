#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse
import re

#from wrapper_conf import (before, beforeTracing, after, attributes, conditions,
#Options)

#files from argv
files = None
#the cfiles to instrument
cFiles = None  
#the signatures of the functios to instrument
functions = None 
#the path to store the output files
output = None
#a prefix to add to the output file name
prefix = None
#a suffix to append to the outputfile name
suffix = None
#if you want to use the force this will be True
force = None
#print out debug informations if True
debug = None

def init():
	"""Initialize the command line parser. Returns a parser from argparse."""

	#FIXME: sort the options
	argParser = argparse.ArgumentParser(description='''Wraps SIOX function
			around MPI function calls''')

	argParser.add_argument('--output', '-o', action='store', nargs=1,
			dest='output', help='out')
	

	argParser.add_argument('--debug', '-d', action='store_true', default=False,
			dest='debug', help='''Print out debug information.''')

	argParser.add_argument('header',metavar='files',action='store', nargs='+',
			help='The header file to instrument.')
	
	return argParser

def add_function():
	"""docstring for add_function"""
	pass

#initialize the command line argument parser 
argParser = init()

#read the command line arguments
args = argParser.parse_args(sys.argv[1:])

#get the parsed arguments
headerFile = args.header
output = args.output 
debug = args.debug


headerFH = open(headerFile[0])
header = headerFH.readlines()

functions = []
for line in header:

	# ^\s* -> leading white space
	# (?!(//)|(/\*))\s* -> exclude comments // or /*
	# (\w+)\s* -> the return value
	# (\*)?\s* -> matches a * if the return value is a pointer don't have to
	#		appear 
	# (\w+)\s* -> matches the function name
	# \( -> matches opening parentheses for the signature
	# (.+) -> matches the parameters of the function 
	# \) -> matches the closing parentheses for the signature
	# \s*;\s*$ -> matches the colon and the trailing white space
	regex = re.match("^\s*(?!(//)|(/\*))\s*(\w+)\s*(\*)?\s*(\w+)\s*\((.+)\)\s*;\s*$", line)

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
	function = functios[-1]

	function.append(returnValue)
	function.append(returnPointer)
	function.append(functionName)
	function.append(signature)
