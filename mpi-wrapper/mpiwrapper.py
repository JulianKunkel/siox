#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8

import __future__
import re
import sys
import argparse

from wrapper_conf import (before, beforeTracing, after, attributes, conditions,
Options)

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
			dest='output', help='''The path where the output will be stored.
			If no path is provided ./ will be used. The output files will have the
			same name as the parsed *.c files. (See --prefix or --suffix)''')
	
	argParser.add_argument('--prefix', '-p', action='store', nargs=1,
			dest='prefix', help='''Add a prefix to the output files. --prefix wrap_
			changes "foo.c" into "wrap_foo.c".''')

	argParser.add_argument('--suffix', '-s', action='store', nargs=1, 
			dest='suffix', help='''Add a suffix to every output file. Could be 
			used to add a different file ending. --suffix .wrap would change 
			"foo.c" into "foo.c.wrap".''')

	argParser.add_argument('--force', action='store_true', default=False,
			dest='force', help='''ONLY USE THIS IF YOU KNOW WHAT YOU ARE DOING! 
			This will ignore warnings and errors. For example allow to override 
			existing *.c files and replace them with the instrumented version of 
			the *.c files or override the existing instrumented *.c files with 
			new one. (There is no short parameter, for a reason!)''')
	
	argParser.add_argument('--debug', '-d', action='store_true', default=False,
			dest='debug', help='''Print out debug information.''')

	argParser.add_argument('files',metavar='files',action='store', nargs='+',
			help='''A list of files containing the function signatures to instrument
			(*.funcs) and the c-flies (*.c) to instrument (order can be mixed).''')
	
	return argParser

def add_function():
	"""docstring for add_function"""
	pass

#initialize the command line argument parser 
argParser = init()

#read the commandline arguments
args = argParser.parse_args(sys.argv[1:])

#get the parsed arguments
files = args.files 
cFiles = [] 
functionFiles = []
functions = [] 
output = args.output 
prefix = args.prefix 
suffix = args.suffix
force = args.force 
debug = args.debug

#print the debug messages to sdterr
if debug: print >> sys.stderr, 'DEBUG: Searching for c- and func-files in the command line arguments:'

#FIXME: Use regex matching, maybe wirte its own function for sorting

#loop over the provided files and sort them to *.c and *.func files
for entry in files:
	try:

		if entry.find('.c') is not -1:

			if debug: print >> sys.stderr, '\tDEBUG: Found c-fiel: %s' % entry
			cFiles.append(entry)

		elif entry.find('.func') is not -1: 
		
			if debug: print >> sys.stderr, '\tDEBUG: Found func-fiel: %s' % entry 
			functionFiles.append(entry)

		else:
		
			print >> sys.stderr, 'ERROR: Unreconised file format: %s' % entry
			
			#if the force option is True skip the exeption
			if force:
					
				print >> sys.stderr, 'Appending the unknown file to the func-files.'
				functionFiles.append(entry)

			else:
				raise SystemError 

	except SystemError:
		exit(1)


