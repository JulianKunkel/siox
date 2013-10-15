#!/usr/bin/env python
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
                               choices=['Boost', 'Protobuf', 'JBinary'],
                               help='''Choose which output-style to use.''')

        argParser.add_argument('--flavor', '-f',
                               action='store', default='xml', dest='flavor',
                               choices=['xml', "text", "binary"],
                               help='''Choose which output-style to use.''')

        argParser.add_argument('--include', '-I', action='store', nargs="+",
                               dest='includeDirs', default=['include', ".", "../include", "../"], help='Include folders')

        argParser.add_argument('--debug', '-d',
                               action='store_true', dest='debug',
                               help='Debug every line')

        argParser.add_argument('--relative', '-r',
                               action='store_true', dest='relative',
                               help='Use relative include path')

        argParser.add_argument('--input-file', '-i', action='store', nargs=1,
                               dest='inputFile', default=None, help='The input file to parse.')

        argParser.add_argument('--output-file', '-o', action='store', nargs=1,
                               dest='outputFile', default=None, help='The output file to write.')

        argParser.add_argument('--output-suffix', '-O', action='store', nargs=1,
                               dest='outputSuffix', default=None, help='Replace the suffix of the read file with the listed suffix.')

        args = argParser.parse_args()

        if not args.inputFile:
            print("Error invalid syntax!")
            os._exit(1)

        args.inputFile = args.inputFile[0]

        if not args.outputSuffix and not args.outputFile:
            args.outputSuffix = [ "." + args.flavor + "Serialization" ]

        # if the outputSuffix is set we create the output in the same directory.

        if args.outputFile :
            args.outputFile =  args.outputFile[0]

        if args.outputSuffix:
            args.outputSuffix = args.outputSuffix[0]
            filename = re.match("(.*)[.]...", args.inputFile)
            args.outputFile = filename.group(1) + args.outputSuffix

	print ("Creating " + args.outputFile);

        return args



def removeNamespace(cls):
    if cls.find("::") >= 0:
        return cls[cls.rfind("::")+2:]
    else:
        return cls

class OutputGenerator():
    fh = None
    parsedIncludes = {}
    typeMap = {}

    def __init__(self):
        pass

    def registerAnnotatedHeader(self, className, parentClasses):
        print(className)
        print(parentClasses)

    def registerMember(self, memberType, memberName, annotations):
        print(memberType)
        print(memberName)


    def registerIntermediatePart(self, text):
        #print(text, file=self.fh)
        m = re.match("\s*namespace ([^{]*)({)?", text)
        if m != None:
            self.namespace[m.group(1).strip()] = 1
            
        # remember typedefs
        m = re.match("\s*typedef\s+(.*)\s+([a-zA-Z0-9_]+)", text)
        if m:
            if self.options.debug:
                print (m.group(1) + " : " + m.group(2))
            self.typeMap[m.group(2)] = m.group(1)


    def registerAnnotatedHeaderEnd(self):
        pass

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
                self.foundIncludeFile(d + "/" + include)
            except IOError:
                continue

            self.nestingDepth = self.nestingDepth - 1
            return

        print("Error, could not read include file \"" + include + "\"")
        self.nestingDepth = self.nestingDepth - 1
        return

    def foundIncludeFile(self, include):
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


class JBinaryOutputGenerator(OutputGenerator):

    def __init__(self):
        self.nestingDepth = 0
        self.mapping = {}
        self.parents = ""
        self.namespace = {}
        self.className = None
        self.registeredTypes = []

    def registerAnnotatedHeader(self, className, parentClasses):
        self.createFileIfNeeded()
        for n in self.namespace:
            #className = "::".join(self.namespace) + "::" + className 
            print("using namespace " + n + ";", file = self.fh)

        self.mapping = {"CLASS" : className , "PARENT" : ",".join(parentClasses), "FLAVOR" : self.flavor }
        self.parents = parentClasses
        self.className = className
        try:
            self.parentClassnames = map(lambda p: re.split("[ \t]+", p)[1].strip(), parentClasses)
        except IndexError:            
            print("Did you forget to specify public class inheritance for class \"" + className + "\" ?")
            os._exit(1)

        if self.options.debug:
            print("\tFound " + className + " parent str: " + str(parentClasses))
            print("\tParents: " + str(self.parentClassnames))

        self.registeredTypes = []

    def map_type_length(self, type, name):
        if type == "bool":
            return "count += 1"    
        if type == "uint8_t":
            return "count += 1"        
        if type == "uint16_t":
            return "count += 2"
        if type == "uint32_t":
            return "count += 4"
        if type == "uint64_t":
            return "count += 8"
        if type == "int8_t":
            return "count += 1"           
        if type == "int16_t":
            return "count += 2"    
        if type == "int32_t":
            return "count += 4"
        if type == "int64_t":
            return "count += 8"
        if type == "double":
            return "count += 8"
        if type == "float":
            return "count += 4" 
        if type == "long double":
            return "count += sizeof(" + name + ")"
        if type.startswith("vector") or type.startswith("list") or type.startswith("std::vector") or type.startswith("std::list"):
            childType = type[ type.find("<") + 1 : -1];
            return "count += 4; \n\tfor(auto itr = %(NAME)s.begin(); itr != %(NAME)s.end() ; itr++){\n\t\t%(CHILD)s; \n\t}"  % {"NAME" : name, "CHILD": self.map_type_length(childType, "*itr")}
        if type.endswith("*"):
            return "if (" + name + " == nullptr ) { count += 1 ; } else{ " + self.map_type_length(type[0:len(type)-2].strip(), "*" + name) + " + 1; }"
        #if type in self.typeMap:
        #    return self.map_memberType(self.typeMap[type])        
        #return "ERROR invalid type: " + type + " for " + name
        return "count += serializeLen(" + name + ")"

    def map_type_serializer(self, type, name):
        if type.startswith("vector") or type.startswith("list") or type.startswith("std::vector") or type.startswith("std::list"):
            childType = type[ type.find("<") + 1 : -1];
            return "\n\t{ uint32_t vlen = %(NAME)s.size(); \n\tserialize(vlen, buffer, pos); \n\tfor(auto itr = %(NAME)s.begin(); itr != %(NAME)s.end() ; itr++){\n\t\t%(CHILD)s; \n\t}}"  % {"NAME" : name, "CHILD": self.map_type_serializer(childType, "*itr")}

        if type.endswith("*"):
            return "if (" + name + " == nullptr ) { serialize((uint8_t) 0, buffer, pos); } else{\n\t\tserialize((uint8_t) 1, buffer, pos); \n\t\t" + self.map_type_serializer(type[0:len(type)-2].strip(), "*" + name) + "; \n\t}"

        return "serialize(" + name + ", buffer, pos)";

    def map_type_deserializer(self, type, name):
        if type.startswith("vector") or type.startswith("list") or type.startswith("std::vector") or type.startswith("std::list"):
            childType = type[ type.find("<") + 1 : -1];
            return "{ uint32_t vlen;  deserialize(vlen, buffer, pos, length);\n\t%(NAME)s.resize(0);\n\tfor(uint32_t i=0; i < vlen; i++){\n\t\t%(CHILDTYPE)s var;\n\t\t%(CHILD)s;\n\t\t%(NAME)s.push_back(var);\n\t}}"  % {"NAME" : name, "TYPE" : type, "CHILD": self.map_type_deserializer(childType, "var"), "CHILDTYPE": childType}

        if type.endswith("*"):
            childType = type[0:len(type)-2].strip()
            return "\n\t{\n\tint8_t ptr; deserialize(ptr, buffer, pos, length);\n\tif ( ptr == 0 ) { %(NAME)s = nullptr; } else{\n\t\t%(NAME)s = (%(TYPE)s) malloc(sizeof(%(CHILDTYPE)s));\n\t\tdeserialize(*%(NAME)s, buffer, pos, length);\n\t}}" % { "NAME" : name, "TYPE" : type, "CHILDTYPE" : childType, "CHILD" : self.map_type_deserializer(childType, "* " + name) }

        return "deserialize(" + name + ", buffer, pos, length)";


    def forceInclude(self, filename):
        self.createFileIfNeeded()
        print("#include <" + filename + "JBinarySerialization.hpp>", file = self.fh)

    def registerAnnotatedHeaderEnd(self):
        if self.options.debug:
            print("\tEnd class")
  
        print("\nnamespace j_serialization{", file=self.fh)

        # create the required methods for (de)serialization
        print("\ninline uint64_t serializeLen(const %(CLASS)s & obj){"  % self.mapping, file = self.fh)
        print("\n\tuint64_t count = 0;", file = self.fh)

        for (memberType, memberName) in self.registeredTypes:
            print("\t" + self.map_type_length(memberType, "obj." + memberName) + ";", file = self.fh)
        
        for p in self.parentClassnames:
            print("count += serializeLen( *( " + p + " *) "  + "this );" , file = self.fh)

        print("\treturn count;", file = self.fh)
        print("}", file = self.fh)

        print("inline void serialize(const %(CLASS)s & obj, char * buffer, uint64_t & pos){"  % self.mapping, file = self.fh)

        for p in self.parentClassnames:
            print("\tserialize( *( " + p + " *) "  + "this );" , file = self.fh)

        for (memberType, memberName) in self.registeredTypes:
            print("\t" + self.map_type_serializer(memberType, "obj." + memberName) + ";", file = self.fh)

        print("}", file = self.fh)

        print("inline void deserialize(%(CLASS)s & obj, const char * buffer, uint64_t & pos, uint64_t length){\n\t"  % self.mapping, file = self.fh)

        for p in self.parentClassnames:
            print("\tdeserialize( *( " + p + " *) "  + "this );" , file = self.fh)

        for (memberType, memberName) in self.registeredTypes:
            print("\t" + self.map_type_deserializer(memberType, "obj." + memberName) + ";", file = self.fh)

        print("}\n", file = self.fh)
        print("}\n", file=self.fh)



    def registerMember(self, memberType, memberName, annotations):
        self.registeredTypes.append([memberType, memberName])

    def initFile(self):
        if self.options.relative:
            dir = os.path.basename(self.options.inputFile)
        else:
            dir = self.options.inputFile

        print("#include \"%(INFILE)s\"" % { "INFILE" : dir } , file=self.fh)

        print("#include <core/container/container-binary-serializer.hpp>\n\n", file=self.fh)

    def finalize(self):
        pass




class BoostOutputGenerator(OutputGenerator):

    def __init__(self):
        self.nestingDepth = 0
        self.mapping = {}
        self.parents = ""
        self.namespace = {}

    def registerAnnotatedHeader(self, className, parentClasses):
        self.createFileIfNeeded()
        for n in self.namespace:
            #className = "::".join(self.namespace) + "::" + className 
            print("using namespace " + n + ";", file = self.fh)

        self.mapping = {"CLASS" : className , "PARENT" : ",".join(parentClasses), "FLAVOR" : self.flavor }
        self.parents = parentClasses
        try:
            self.parentClassnames = map(lambda p: re.split("[ \t]+", p)[1].strip(), parentClasses)
        except IndexError:            
            print("Did you forget to specify public class inheritance for class \"" + className + "\" ?")
            os._exit(1)


        if self.options.debug:
            print("\tFound " + className + " parent str: " + str(parentClasses))
            print("\tParents: " + str(self.parentClassnames))


        print("""
            #ifndef BOOST_SERIALIZABLE_%(CLASS)s
            #define BOOST_SERIALIZABLE_%(CLASS)s
            namespace boost{
            namespace serialization {
            template<class Archive>
            void serialize( Archive & ar, %(CLASS)s & a, const unsigned int version )
            {
                """  % self.mapping , file = self.fh)

    def forceInclude(self, filename):
        self.createFileIfNeeded()
        print("#include <%sBoostSerialization.hpp>" % (filename), file=self.fh)

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

        print("""
            BOOST_CLASS_EXPORT(%(CLASS)s)
            BOOST_CLASS_IMPLEMENTATION(%(CLASS)s, boost::serialization::object_serializable)
            BOOST_CLASS_TRACKING(%(CLASS)s, boost::serialization::track_never)""" % self.mapping , file = self.fh)
        
        if self.nestingDepth == 0:
            print("""template void boost::serialization::serialize(boost::archive::%(FLAVOR)s_oarchive & ar, %(CLASS)s & g, const unsigned int version);
                template void boost::serialization::serialize(boost::archive::%(FLAVOR)s_iarchive & ar, %(CLASS)s & g, const unsigned int version);
                """ % self.mapping , file = self.fh)
        print("\n#endif", file = self.fh)


        

    def registerMember(self, memberType, memberName, annotations):
        if self.flavor == "xml":
            print("""\t\tar & boost::serialization::make_nvp("%(MEMBER)s", a.%(MEMBER)s);""" % {"MEMBER" : memberName} , file = self.fh)
        else:
            print("""\t\tar & a.%(MEMBER)s;""" % {"MEMBER" : memberName} , file = self.fh)


    def initFile(self):
        
        if self.options.relative:
            dir = os.path.basename(self.options.inputFile)
        else:
            dir = self.options.inputFile
        print("""#include <boost/serialization/serialization.hpp>
                \n #include <boost/serialization/nvp.hpp>
                \n #include <boost/serialization/vector.hpp>
                \n #include <boost/serialization/map.hpp>
                \n #include <boost/serialization/list.hpp>
                \n #include <boost/serialization/tracking.hpp>
                \n #include <boost/serialization/level.hpp>
                \n #include <boost/serialization/export.hpp>
                \n #include <boost/archive/%(FLAVOR)s_iarchive.hpp>
                \n #include <boost/archive/%(FLAVOR)s_oarchive.hpp>                               
                \n #include \"%(INFILE)s\" 

                """ % { "INFILE" : dir, "FLAVOR" : self.options.flavor } , file=self.fh)



class ProtoBufOutputGenerator(OutputGenerator):

    def __init__(self):
        self.nestingDepth = 0
        self.mapping = {}
        self.parents = ""
        self.namespace = {}
        self.className = None
        self.convertFileHPP = None
        self.convertFileCPP = None
        self.registeredTypes = []

    def createFileIfNeeded(self):
        if self.fh == None:
            self.fh = open(self.options.outputFile + ".proto", "w")
            self.convertFileCPP = open(self.options.outputFile + ".cpp", "w")
            self.convertFileHPP = open(self.options.outputFile + ".hpp", "w")
            self.initFile()


    def initFile(self):
        if self.options.relative:
            dir = os.path.basename(self.options.inputFile)
        else:
            dir = self.options.inputFile
        print("#include <" + dir + ">\n", file = self.convertFileHPP)
        print("#include <" + self.options.outputFile + ".pb.h>\n", file = self.convertFileHPP)

        print("#include <" + self.options.outputFile + ".hpp>\nnamespace convert{", file = self.convertFileCPP)

        print("""package protobuf;\n"""  , file=self.fh)
        for n in self.namespace:
            #className = "::".join(self.namespace) + "::" + className 
            print("using namespace " + n + ";", file = self.convertFileCPP)            

    def finalize(self):
        print("}", file = self.convertFileCPP)
        self.convertFileCPP.close()
        self.convertFileHPP.close()


    def foundIncludeFile(self, include):
        self.createFileIfNeeded()
        print("#include <" + include + ">", file = self.convertFileHPP)


    def map_memberType(self, type):
        if type == "uint16_t":
            return "uint32"    
        if type == "uint32_t":
            return "uint32"
        if type == "uint64_t":
            return "uint64" 
        if type.startswith("vector"):
            return self.map_memberType(type[7:-1]);
        if type.endswith("*"):
            return self.map_memberType(type[0:len(type)-2].strip());
        if type in self.typeMap:
            return self.map_memberType(self.typeMap[type])
        return type

    def protobuf_map_qualifier(self, type):
        if type.startswith("vector"):
            return "repeated";
        if type.endswith("*"):
            return "optional";
        return "required"
            
    def registerAnnotatedHeader(self, className, parentClasses):
        self.createFileIfNeeded()

        self.mapping = {"CLASS" : className , "PARENT" : ",".join(parentClasses)}
        print("""\tmessage %(CLASS)s {
                """  % self.mapping , file = self.fh)
        self.className = className

        self.registeredTypes = []

        self.currentNumber = 1;

    def forceInclude(self, filename):
        self.createFileIfNeeded()
        fh = open(filename + ".proto")
        data = fh.read()
        print(data, file=self.fh)
        fh.close()


    def registerAnnotatedHeaderEnd(self):        
        print("""\t}\n"""  % self.mapping, file = self.fh)
        # create converter between protobuf and regular in-memory container.
        for n in self.namespace:
            #className = "::".join(self.namespace) + "::" + className 
            print("using namespace " + n + ";", file = self.convertFileHPP)

        print("""namespace convert{
            %(CLASS)s convertToSerializable(const protobuf::%(CLASS)s & protobuf);
            protobuf::%(CLASS)s convertToProto(const %(CLASS)s & serializable);
            }
            """  % self.mapping, file = self.convertFileHPP)

        if self.options.debug:
            print(self.registeredTypes)

        # Implement the converter methods:
        print("%(CLASS)s convertToSerializable(const protobuf::%(CLASS)s & protobuf){"
             % self.mapping, file = self.convertFileCPP)
        for type in self.registeredTypes:
            print(type)
        print("}", file = self.convertFileCPP)

        print("protobuf::%(CLASS)s convertToProto(const %(CLASS)s & serializable){"  
            % self.mapping, file = self.convertFileCPP)
        for type in self.registeredTypes:
            pass
        print("}", file = self.convertFileCPP)



        

    def registerMember(self, memberType, memberName, annotations):
            print("""\t\t %(QUALIFIER)s %(TYPE)s %(MEMBER)s = %(NUMBER)s;""" % {"QUALIFIER": self.protobuf_map_qualifier(memberType), "MEMBER" : memberName, "NUMBER" : self.currentNumber, "TYPE": self.map_memberType(memberType)
                } , file = self.fh)
            self.currentNumber = self.currentNumber + 1
            self.registeredTypes.append([memberType, memberName])


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
    containerNesting = 0
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
                m = re.search("(class|struct) ([a-zA-Z_0-9]+)\s*(:\s*([^{]*))?\s*({?)", lines[lineNR+1])

                assert(m)

                if m.group(4) != None:
                    parentClasses = m.group(4).replace("ComponentOptions", "Container").split(",")
                else:
                    parentClasses = []

                output_generator.registerAnnotatedHeader(m.group(2).strip(), parentClasses)

                lineNR = lineNR + 1

                containerNesting = 0

                continue
            elif command == "noserialization":
                ignoreNextMember = True
                continue
            elif command == "externalserialization":
                # We have to include the appropriate header file.
                if options.debug:
                    print("Found external serialization tag in " + file)
                filename = re.match("(../include/)?(.*)([.]...)", file)
                output_generator.forceInclude(filename.group(2))
                continue
            else:
                annotations.append(command)
                continue
        
        if containerMode:            
            #print (str(containerNesting) + " " + line )

            # Count "{"
            if line.find("}") > -1:
                containerNesting = containerNesting - 1

                if containerNesting == 0:
                    output_generator.registerAnnotatedHeaderEnd()
                    containerMode = False
                    continue

            if line.find("{") > -1:
                containerNesting = containerNesting + 1

            if containerNesting == 1:
                # Parse a member
                m = re.search("\s*([^=]*)\s*[ \t]([a-zA-Z_][a-zA-Z_]*)\s*(=.*)?;", line)
                if not m:
                    continue

                if ignoreNextMember:
                    annotations = []
                    ignoreNextMember = False
                    continue

                output_generator.registerMember(m.group(1).strip(), m.group(2), annotations)
                annotations = []
                continue
            continue

        output_generator.registerIntermediatePart(line)

def main():
    opt = Option()
    options = opt.parse()

    if options.style == "Protobuf":
        og = ProtoBufOutputGenerator()
    elif options.style == "JBinary":
        og = JBinaryOutputGenerator()
    elif options.style == "Boost":
        og = BoostOutputGenerator()

    og.setOptions(options)

    parseFile(options.inputFile, options, og)
    og.finalize()

if __name__ == '__main__':
    main()
