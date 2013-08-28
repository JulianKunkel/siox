#!/usr/bin/python2
# -*- coding: utf-8 -*-
# encoding: utf-8
from __future__ import print_function

import re
import argparse
import os

class Option():

    #
    # @brief Parse the command line arguments.
    #
    # Parses the command line arguments, using the python argparser.
    def parse(self):
        prog = 'container code generator'

        description = '''This application parses all containers within the source tree and creates classes to serialize/deserialize the containers. Different flavors of containers such as boost serialization and google protocol buffers are offered.'''

        argParser = argparse.ArgumentParser(description=description, prog=prog)

        # argParser.add_argument('--cpp-args', '-a', action='store', nargs=1, default=[],
        # dest='cppArgs', help='''Pass arguments to the cpp.''')

        argParser.add_argument('--style', '-s',
                               action='store', default='Boost', dest='style',
                               choices=['Boost', 'Protobuf'],
                               help='''Choose which output-style to use.''')

        argParser.add_argument('--flavor', '-f',
                               action='store', default='xml', dest='flavor',
                               choices=['xml', "text"],
                               help='''Choose which output-style to use.''')

        argParser.add_argument('--include', '-I', action='store', nargs="+",
                               dest='includeDirs', default=['include', "."], help='Include folders')

        argParser.add_argument('--debug', '-d',
                               action='store_true', dest='debug',
                               help='Debug every line')

        argParser.add_argument('--input-file', '-i', action='store', nargs=1,
                               dest='inputFile', default='not-defined', help='The input file to parse.')

        argParser.add_argument('--output-file', '-o', action='store', nargs=1,
                               dest='outputFile', default=None, help='The output file to write.')

        argParser.add_argument('--output-suffix', '-O', action='store', nargs=1,
                               dest='outputSuffix', default=None, help='Replace the suffix of the read file with the listed suffix.')

        args = argParser.parse_args()

        args.inputFile = args.inputFile[0]

        if not args.outputSuffix and not args.outputFile:
            args.outputSuffix = [ "." + args.flavor + "Serialization" ]

        # if the outputSuffix is set we create the output in the same directory.

        if args.outputSuffix:
            args.outputSuffix = args.outputSuffix[0]
            filename = re.match("(.*)[.]...", args.inputFile)
            args.outputFile = filename.group(1) + args.outputSuffix

        return args



def removeNamespace(cls):
    if cls.find("::") >= 0:
        return cls[cls.rfind("::")+2:]
    else:
        return cls

class OutputGenerator():
    fh = None

    def __init__(self):
        pass

    def registerAnnotatedHeader(self, className, parentClasses):
        print(className)
        print(parentClasses)

    def registerMember(self, memberType, memberName, annotations):
        print(memberType)
        print(memberName)

    def registerIntermediatePart(self, text):
        pass

    def registerAnnotatedHeaderEnd(self):
        pass

    def registerInclude(self, include):
        pass

    def setOptions(self, options):
        self.options = options
        self.flavor = options.flavor

    def createFileIfNeeded(self):
        if self.fh == None:
            self.fh = open(self.options.outputFile, "w")
            self.initFile()

    def initFile(self):
        pass

    def finalize(self):
        pass

    def forceInclude(self, filename):
        pass

class BoostOutputGenerator(OutputGenerator):

    def __init__(self):
        self.nestingDepth = 0
        self.mapping = {}
        self.parents = ""
        self.namespace = {}
        self.parsedIncludes = {}

    def registerAnnotatedHeader(self, className, parentClasses):
        self.createFileIfNeeded()
        for n in self.namespace:
            #className = "::".join(self.namespace) + "::" + className 
            print("using namespace " + n + ";", file = self.fh)

        self.mapping = {"CLASS" : className , "PARENT" : ",".join(parentClasses), "FLAVOR" : self.flavor }
        self.parents = parentClasses
        self.parentClassnames = map(lambda p: re.split("[ \t]+", p)[1].strip(), parentClasses)


        if self.options.debug:
            print("\tFound " + className + " parent str: " + str(parentClasses))
            print("\tParents: " + str(self.parentClassnames))


        print("""
            namespace boost{
            namespace serialization {
            template<class Archive>
            void serialize( Archive & ar, %(CLASS)s & a, const unsigned int version )
            {
                """  % self.mapping , file = self.fh)

    def forceInclude(self, filename):
        print("#include <%s>" % (filename), file = self.fh)

    def registerInclude(self, include):
        # Recursively check include file for required serialization objects
        if include in self.parsedIncludes:
            return
        
        self.parsedIncludes[include] = 1

        self.nestingDepth = self.nestingDepth + 1

        for d in self.options.includeDirs:
            # If we cannot parse the file we check the other files
            try:                
                parseFile(d + "/" + include, self.options, self)
            except IOError:
                continue

            self.nestingDepth = self.nestingDepth - 1
            return

        print("Error, could not read include file \"" + include + "\"")
        self.nestingDepth = self.nestingDepth - 1
        return


    def registerAnnotatedHeaderEnd(self):
        if self.options.debug:
            print("\tEnd class")
        if self.flavor == "xml":
            for p in self.parentClassnames:            
                print(""" 
                ar & boost::serialization::make_nvp("%(PARENT_NO_NAMESPACE)s", boost::serialization::base_object<%(PARENT)s>(a)); """ % {"PARENT" : p, "PARENT_NO_NAMESPACE" : removeNamespace(p) }, file = self.fh)
        else:
            for p in self.parentClassnames:            
                print(""" 
                ar & boost::serialization::base_object<%(PARENT)s>(a)); """ % {"PARENT" : p }, file = self.fh)

        print("""            
            }
            }
            }"""  % self.mapping, file = self.fh)
        

    def registerMember(self, memberType, memberName, annotations):
        if self.flavor == "xml":
            print("""\t\tar & boost::serialization::make_nvp("%(MEMBER)s", a.%(MEMBER)s);""" % {"MEMBER" : memberName} , file = self.fh)
        else:
            print("""\t\tar & a.%(MEMBER)s;""" % {"MEMBER" : memberName} , file = self.fh)


    def registerIntermediatePart(self, text):
        #print(text, file=self.fh)
        m = re.match("\s*namespace ([^{]*)({)?", text)
        if m != None:
            self.namespace[m.group(1).strip()] = 1


    def initFile(self):
        print("""#include <boost/serialization/serialization.hpp>
                \n #include <boost/serialization/nvp.hpp>
                \n #include <boost/serialization/vector.hpp>
                \n #include <boost/serialization/map.hpp>
                \n #include <boost/serialization/tracking.hpp>
                \n #include <boost/serialization/level.hpp>
                \n #include <boost/serialization/export.hpp>
                \n #include <boost/archive/%(FLAVOR)s_iarchive.hpp>
                \n #include <boost/archive/%(FLAVOR)s_oarchive.hpp>                                
                \n #include \"%(INFILE)s\" 

                """ % { "INFILE" : os.path.basename(self.options.inputFile), "FLAVOR" : self.options.flavor } , file=self.fh)


    def finalize(self):
        print("""
            BOOST_CLASS_EXPORT(%(CLASS)s)
            BOOST_CLASS_IMPLEMENTATION(%(CLASS)s, boost::serialization::object_serializable)
            BOOST_CLASS_TRACKING(%(CLASS)s, boost::serialization::track_never)""" % self.mapping , file = self.fh)
        if self.nestingDepth == 0:
            print("""template void boost::serialization::serialize(boost::archive::%(FLAVOR)s_oarchive & ar, %(CLASS)s & g, const unsigned int version);
                template void boost::serialization::serialize(boost::archive::%(FLAVOR)s_iarchive & ar, %(CLASS)s & g, const unsigned int version);
                """ % self.mapping , file = self.fh)


def parseFile(file, options, output_generator):
    fh = open(file)
    string = fh.read()
    fh.close()
    if options.debug:
        print("Parsing " + file)
    
    # Clean input
    string = re.sub('/\*.*?\*/', '', string, flags=re.M | re.S)
    # Remove all comments which provide no annotations
    string = re.sub('//[^@].*', '', string)

    commandRegex = re.compile("//@(.*)\s*")

    # A valid include must contain at least one /
    includeRegex = re.compile('\s*#include [<"]((?!boost).*/.*)[>"]\s*');
    
    containerMode = False
    ignoreNextMember = False

    lines = string.split("\n")

    annotations = []

    foundAnnotation = False

    for lineNR in range(0, len(lines)-1):
        line = lines[lineNR]

        # Check for includes
        includeMatch = includeRegex.match(line);
        if includeMatch:
            output_generator.registerInclude(includeMatch.group(1).strip())
            continue

        # Check for commands
        commandMatch = commandRegex.search(line)
        if commandMatch:
            command = commandMatch.group(1).lower()
            if command == "serializable":
                foundAnnotation = True
                assert(containerMode == False)
                containerMode = True
                # The next line is expected to be the class definition
                m = re.search("class ([^:]*)(: (.*))? [{]", lines[lineNR+1])
                assert(m)
                if m.group(2) != None:
                    parentClasses = m.group(3).replace("ComponentOptions", "Container").split(",")
                else:
                    parentClasses = []

                output_generator.registerAnnotatedHeader(m.group(1).strip(), parentClasses)       

                lineNR = lineNR + 1
                continue
            elif command == "noserialization":
                ignoreNextMember = True
                continue
            elif command == "externalserialization":
                # We have to include the appropriate header file.
                if options.debug:
                    print("Found external serialization tag in " + file)
                filename = re.match("(.*)([.]...)", file)
                output_generator.forceInclude(filename.group(1) + options.style + "Serialization" + filename.group(2))
                continue
            else:
                annotations.append(command)
                continue
        
        if containerMode:
            if line.find("}") > -1:
                output_generator.registerAnnotatedHeaderEnd()
                containerMode = False
                continue
            else:
                # Parse a member
                m = re.search("\s*(.*)\s*[ \t]([a-zA-Z_][a-zA-Z_]*)\s*(=.*)?;", line)
                if not m:
                    continue

                if ignoreNextMember:
                    annotations = []
                    ignoreNextMember = False
                    continue

                output_generator.registerMember(m.group(1).strip(), m.group(2), annotations)
                annotations = []
                continue

        output_generator.registerIntermediatePart(line)

    if foundAnnotation:
        output_generator.finalize()


def main():
    opt = Option()
    options = opt.parse()

    #matchingHeaders = []
    #for root, dirnames, filenames in os.walk(options.inputDir):
    #  for filename in fnmatch.filter(filenames, '*.hpp'):
    #      matches.append(os.path.join(root, filename))

    og = BoostOutputGenerator()

    og.setOptions(options)

    parseFile(options.inputFile, options, og)

if __name__ == '__main__':
    main()
