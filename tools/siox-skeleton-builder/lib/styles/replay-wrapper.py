
from __future__ import print_function

import skeletonBuilder

class Style(skeletonBuilder.Writer):
    #
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def writeOutput(self, options, functionList, templateParameters, precompiler):
        # open the output file for writing
        output = open(options.outputFile, 'w')
        print("/**\n * GENERATED PLUGIN! - Modify carefully. May loose changes on regeneration. \n */", end='\n', sep='', file=output)


        templateParameters["includes"].append("<map>")
        templateParameters["includes"].append("<iostream>")
        templateParameters["includes"].append("\"wrapper.h\"")
        templateParameters["includes"].append("\"datatypes.h\"")

        # write all needed includes
        for match in templateParameters["includes"]:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)   


        print("""/**
 * Map the trace filehandles to actual file handles
 */
int issued = 3;	// 3 will be first issued by this replayer

std::map<int,int> fds;
std::map<int,FILE> streams;""", file=output)

        # create global template code
        print("// GENERATED FROM TEMPLATE GLOBAL \n", file=output)
        for function in functionList:
            functionVariables = self.functionVariables(function)

            for templ in function.usedTemplateList:
                outputString = templ.output('global', functionVariables)
                if outputString != '':
                    print(outputString, end='\n', sep='', file=output)
        print("// GENERATED FROM TEMPLATE GLOBAL END\n", file=output)


        print("""
void dump_map() {
	printf("dump_map()\\n");
	for (auto it=activityHashTable_int.begin(); it!=activityHashTable_int.end(); ++it) {
        std::cout << " dm: " << it->first << " => " << it->second << '\\n';
	}
	printf("dump_map() end \\n");
}
                """, file=output)


        for function in functionList:
            functionVariables = self.functionVariables(function)
            # write function signature

            print("int wrapped_", function.name, "(void * data)", sep='', file=output)
            print("{\n", "\t", "posix_", function.name ,"_data * d = (posix_", function.name  ,"_data*) data;", sep='', file=output)

            print("\tint ret =-1;\n", file=output)

            print("// GENERATED FROM TEMPLATE\n", file=output)
            sections = ["feign_wrapper_before", "init", "feign_wrapper_after"]
            # insert the previous sections in that order
            for sectionName in sections:
                for templ in function.usedTemplateList:
                    outputString = templ.output(sectionName, functionVariables)
                    if outputString != '':
                        print('\t', outputString, end='\n', sep='', file=output)
            print("// GENERATED FROM TEMPLATE END\n", file=output)

            print("\t","FEIGN_LOG(2,\"'-", function.name, "()\");", sep='', file=output)
            print("\t","return 0;\n}", sep='', file=output)



        # close the file
        output.close()
