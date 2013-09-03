#!/usr/bin/env python
# encoding: utf-8

# Waf documentation, see http://docs.waf.googlecode.com/git/book_17/single.html
# Build and run unit tests:
# See: http://docs.waf.googlecode.com/git/apidocs_16/tools/waf_unit_test.html#module-waflib.Tools.waf_unit_test

# Waf has been build using: ./waf-light --tools=boost,doxygen,eclipse,proc,review,why

from waflib import Logs
from waflib import Options
#from waflib.Tools import c_preproc
from waflib import Task
from waflib import TaskGen
from waflib.TaskGen import feature, before_method

import fnmatch
import mmap
import os
import re

@feature('*')
@before_method('process_rule')
def post_the_other(self):
    deps = getattr(self, 'depends_on', [])
    for name in self.to_list(deps):
        other = self.bld.get_tgen_by_name(name)
        other.post() 

def __recurse(ctx):
	workDir = ctx.path.abspath()
	dirs = [ name for name in os.listdir(workDir)    if   os.path.isdir(os.path.join(workDir, name)) and not name.startswith(".") ]
	Logs.debug("walk into " + workDir)
	ctx.recurse(dirs, mandatory=False)

def options(opt):
	opt.load('compiler_cxx waf_unit_test compiler_c')
	opt.load('boost')

        gr = opt.get_option_group('configure options')
	#bo = opt.add_option_group("Build options")
	gr.add_option('--production', action='store_true', default=False, dest='production', help="Disable debugging mode")
	gr.add_option('-d', action='store_true', default=False, dest='debug', help="Enable debugging mode")
	gr.add_option('--dummyCPPImplementations', action='store_true', default=False, dest='dummyCPPImplementations', help="Build dummy modules/plugins for all supporting layers")

	__recurse(opt)

def configure(conf):
	conf.load('compiler_cxx')
	conf.load('compiler_c')
	conf.load('waf_unit_test')
	conf.load('boost')

	# Locate Plantuml for UML diagrams in documentation
	conf.find_program('/usr/share/plantuml/plantuml.jar', var='PLANTUML', mandatory=False)
	#conf.find_file('plantuml.jar', ['/usr'])
	if not "PLANTUML" in conf.env:
		print "\tI cannot create the UML images for the API documentation, but please proceed!"
	# Locate Doxygen for documentation
	conf.find_program('doxygen', var='DOXYGEN', mandatory=False)
	if not "DOXYGEN" in conf.env:
		print "\tI cannot create the API documentation, but please proceed!"

	# Locate astyle for style command
	conf.find_program("astyle", var='ASTYLE', mandatory=False)
	if not "ASTYLE" in conf.env:
		print "\tI cannot apply siox style to the source tree, but please proceed!"

	# Check various packages for correct installations and linkage
	conf.check_cfg(package='glib-2.0', uselib_store='GLIB',  args=['--cflags', '--libs'], mandatory=True)
	conf.check_cfg(package='gmodule-2.0', use="GLIB", uselib_store='GMODULES',   args=['--cflags', '--libs'], mandatory=True)

	conf.check_cfg(package='libpqxx', uselib_store='PQXX',   args=['--cflags', '--libs'], mandatory=True)

	# Check Boost library for correct installation and linkage
	conf.check_boost(lib='system thread serialization regex program_options')

	conf.env.append_value("RPATH", conf.env.PREFIX + "/lib")

	workDir = conf.path.abspath()
	conf.env.append_value('INCLUDES', [workDir + '/include' ])
        if conf.options.debug:
	        conf.env.CXXFLAGS = ['-std=c++11', '-O3', '-Wall']
		conf.env.append_value("CFLAGS", ['-std=c99', '-O3', '-Wall'])
	else:
		conf.env.CXXFLAGS = ['-std=c++11', '-g', '-Wall']
		conf.env.append_value("CFLAGS", ['-std=c99', '-g', '-Wall'])

	conf.env.CODEGEN=conf.path.abspath() + "/tools/serialization-code-generator.py"

	print ""
	__recurse(conf)
	print "Debugging:                               : %s" % conf.options.debug

def doc(ctx):
	"""Create developer's documentation, see doc/index.html. Requires doxygen."""
	# Run plantuml to create *.png files from embedded code.
	# Files are placed in ./doc/images/plantuml
	# Recusively search from current folder scanning files fittin the file patterns
	# ctx.exec_command("java  -Djava.awt.headless=true -jar ${PLANTUML} -v -o ${PWD}/doc/images  ./**.(c|cpp|doxygen|h|hpp|uml) >plantuml.log")
	ctx.exec_command("plantuml -v -o ${PWD}/doc/images  './**.(doxygen|h|hpp|uml)' >plantuml.log")
	ctx.exec_command("doxygen")


def build(bld):
	# Allow autogeneration of dummy C++ classes:
	def dummyPlugin(task): 
                src = task.inputs[0].abspath()
                tgt = task.outputs[0].abspath()
		Logs.debug(src)
                cmd = bld.path.abspath() + '/tools/c++dummy-implementer.py -o %s --style %s %s' % (tgt, "cout", src)
                return task.exec_command(cmd)

	def dummyListPlugin(task): 
                src = task.inputs[0].abspath()
                tgt = task.outputs[0].abspath()
                cmd = bld.path.abspath() + '/tools/c++dummy-implementer.py -o %s --style %s %s' % (tgt, "list", src)
                return task.exec_command(cmd)
	
	def dummyCPP(self, source, target):
		Logs.debug(source)
		bld(rule=dummyPlugin, source=source, target=target + "DummyCout.cpp")
		bld.shlib(includes="", source = target+ "DummyCout.cpp", target = target + "DummyPrintf")
		bld(rule=dummyListPlugin, source=source, target=target + "DummyList.cpp")
		bld.shlib(includes="", source = target + "DummyList.cpp", target = target + "DummyList")

	bld.dummyCPP = dummyCPP


	bld.recurse(['core'], mandatory=True)
	bld.recurse(['monitoring'], mandatory=True)
	bld.recurse(['knowledge'], mandatory=True)
	bld.recurse(['tools'])

	# build test interfaces
	# Manual usage would be:
	#bld.dummyCPP(bld, source="include/monitoring/ontology/Ontology.hpp", target='monitoring/ontology/modules/Ontology')
	if Options.options.dummyCPPImplementations:
	  test_interface_re = re.compile("BUILD_TEST_INTERFACE\s+([^ \n]+)",  re.M)
	  for root, dirnames, filenames in os.walk('include'):
	    for filename in fnmatch.filter(filenames, '*.hpp'):
		  fqfilename = os.path.join(root, filename)
		  lastlines = tail(fqfilename)
		  if lastlines == 0:
			  continue
		  match = test_interface_re.search(lastlines)
		  if match != None:
			  target = match.group(1)
			  #print (fqfilename + " " + target)
			  bld.dummyCPP(bld, source=fqfilename, target=target)


	from waflib.Tools import waf_unit_test
        bld.add_post_fun(waf_unit_test.summary)

	# installation of header files
	for root, dirs, files in os.walk("include"):
		for f in files:
			bld.install_files("${PREFIX}/" + root, root + "/" + f)


def style(ctx):
	"""Format entire source tree according to SIOX conventions, see scripts/siox.astyle. Requires astyle."""
	# Run astyle to format entire project source code tree according to SIOX style guidelines
	# as defined in file siox.astyle
	# 
	ctx.exec_command('astyle --recursive --options=scripts/siox.astyle "*.h" "*.c" "*.hpp" "*.cpp"')
	#" './*.h'")
	# Remove backup files
	#ctx.exec_command("find . -name *.[hc]pp.orig -or -name *.[hc].orig -delete")
	pass


# Code from Stackoverflow:
def tail(filename, n = 5):
    """Returns last n lines from the filename. No exception handling"""
    size = os.path.getsize(filename)
    if size < 10: 
	return ""
    with open(filename, "rb") as f:
        # for Windows the mmap parameters are different
        fm = mmap.mmap(f.fileno(), 0, mmap.MAP_SHARED, mmap.PROT_READ)
        try:
            for i in xrange(size - 1, -1, -1):
                if fm[i] == '\n':
                    n -= 1
                    if n == -1:
                        break
            return fm[i + 1 if i else 0:]
        finally:
            fm.close()

