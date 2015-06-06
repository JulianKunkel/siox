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

        print("""#ifndef WRAPPER_H_CJ0RYGIY\n#define WRAPPER_H_CJ0RYGIY""", file=output)
       
        # drop all template includes
        templateParameters["includes"] = []
        templateParameters["includes"].append("<feign.h>")
        templateParameters["includes"].append("\"datatypes.h\"")

        # write all needed includes
        for match in templateParameters["includes"]:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)   
        
        print('void dump_map();\n\n', file=output)   

        # declare function prototypes
        for function in functionList:
            functionVariables = self.functionVariables(function)
            # write function signature

            print("int wrapped_", function.name, "(void * data);", end='\n', sep='', file=output)


        print("""#endif /* end of include guard: WRAPPER_H_CJ0RYGIY */""", file=output)

        # close the file
        output.close()
