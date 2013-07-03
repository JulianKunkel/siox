#!/usr/bin/env python
# encoding: utf-8

# Waf documentation, see http://docs.waf.googlecode.com/git/book_16/single.html
# Build and run unit tests:
# See: http://docs.waf.googlecode.com/git/apidocs_16/tools/waf_unit_test.html#module-waflib.Tools.waf_unit_test

# Waf has been build using: ./waf-light --tools=boost,doxygen,eclipse,proc,review,why

from waflib import Logs

import os

def __recurse(ctx):
	workDir = ctx.path.abspath()
	dirs = [ name for name in os.listdir(workDir)    if   os.path.isdir(os.path.join(workDir, name)) and not name.startswith(".") ]
	Logs.debug("walk into " + workDir)
	ctx.recurse(dirs, mandatory=False)

def options(opt):
	opt.load('compiler_cxx waf_unit_test compiler_c')
	opt.load('boost')

	#bo = opt.add_option_group("Build options")
	opt.add_option('--production', action='store_true', default=False, dest='production', help="Disable debugging mode")
	opt.add_option('--doc', action='store_true', default=False, dest='debug', help="Enable debugging mode")

	__recurse(opt)

def configure(conf):
	conf.load('compiler_cxx')
	conf.load('compiler_c')
	conf.load('waf_unit_test')
	conf.load('boost')

	conf.find_program('doxygen', var='DOXYGEN', mandatory=False)
	if not "DOXYGEN" in conf.env:
		print "\tI cannot create the API documentation, but please proceed!"

	conf.check_cfg(package='glib-2.0', uselib_store='GLIB',  args=['--cflags', '--libs'], mandatory=True)
	conf.check_cfg(package='gmodule-2.0', use="GLIB", uselib_store='GMODULES',   args=['--cflags', '--libs'], mandatory=True)

	conf.check_cfg(package='libpqxx', uselib_store='PQXX',   args=['--cflags', '--libs'], mandatory=True)


        conf.check_boost(lib='system thread serialization')


	workDir = conf.path.abspath()
	conf.env.INCLUDES = [workDir + '/include' ]

        if conf.options.production:
	        conf.env.CXXFLAGS = ['-std=c++11', '-O3', '-Wall']
	else:
		conf.env.CXXFLAGS = ['-std=c++11', '-g', '-Wall']


	print ""
	__recurse(conf)
	print "Debugging:                               : %s" % conf.options.debug

def doc(ctx):
	ctx.exec_command("doxygen")

def build(bld):
	#__recurse(bld)
	bld.recurse(['core'], mandatory=True)
#	bld.recurse(['core', 'monitoring'], mandatory=True)
#	bld.recurse(['core', 'monitoring', 'knowledge'], mandatory=True)
	from waflib.Tools import waf_unit_test
        bld.add_post_fun(waf_unit_test.summary)

	# installation of header files
	for root, dirs, files in os.walk("include"):
		for f in files:
			bld.install_files("${PREFIX}/" + root, root + "/" + f)


# -*- indent-tabs-mode: t -*-
