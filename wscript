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
	opt.load('compiler_cxx waf_unit_test')

	#bo = opt.add_option_group("Build options")
	opt.add_option('--debug', action='store_true', default=False, dest='debug', help="Enable debugging mode")
	opt.load('boost')

	__recurse(opt)
	
def configure(conf):
	conf.load('compiler_cxx')
        conf.load('waf_unit_test')
	conf.load('boost')

        conf.check_boost(lib='system thread-mt')


	workDir = conf.path.abspath()
	conf.env.INCLUDES = [workDir + '/include' ]

        if conf.options.debug:
		conf.env.CXXFLAGS = ['-std=c++11', '-g', '-Wall'] 
	else:
	        conf.env.CXXFLAGS = ['-std=c++11', '-O3', '-Wall'] 

	conf.check_cfg(package='glib-2.0', uselib_store='GLIB',  args=['--cflags', '--libs'], mandatory=True)
	conf.check_cfg(package='gmodule-2.0', use="GLIB", uselib_store='GMODULES',   args=['--cflags', '--libs'], mandatory=True)

	print ""
	__recurse(conf)
	print "Debugging:                               : %s" % conf.options.debug

	
def build(bld):
	#__recurse(bld)
	bld.recurse(['core'], mandatory=True)
	from waflib.Tools import waf_unit_test
        bld.add_post_fun(waf_unit_test.summary)


def install(ctx):
	__recurse(ctx)

def clean(ctx):
	__recurse(ctx)

def distcheck(ctx):
	__recurse(ctx)

def distclean(ctx):
	__recurse(ctx)

def step(ctx):
	__recurse(ctx)

def uninstall(ctx):
	__recurse(ctx)



# -*- indent-tabs-mode: t -*-
