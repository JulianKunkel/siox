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

        # write all needed includes
        for match in templateParameters["includes"]:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)

        # write all global-Templates
        for function in functionList:
            functionVariables = self.functionVariables(function)

            for templ in function.usedTemplateList:
                if templ.output('global', functionVariables) != '':
                    print(templ.output('global', functionVariables), file=output)
        print("", file=output)

        # write global once from template
        print(templateParameters["globalOnce"], file=output)

        # write all functions-bodies
        for function in functionList:
            functionVariables = self.functionVariables(function)

            # write function signature

            print(function.getDefinition(),
                  end='\n{\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print('\tva_list valist;', file=output)
                print('\tva_start(valist, %s);' % function.parameterList[-2].name, file=output)
                #print(                    '\t%s val = va_arg(valist, %s);' % (function.parameterList[-2].type,function.parameterList[-2].type), file=output)
                # set the name to args
                function.parameterList[-1].name = "val"

            # a variable to save the return-value
            returnType = function.type

            if returnType != "void":
                print('\t', returnType, ' ret;', end='\n', sep='',
                      file=output)


            self.writeBefore(function, output, functionVariables)
            self.writeAfter(function, output, functionVariables)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                print("\tva_end(valist);", file=output)

            # write the return statement and close the function
            if returnType != "void":
                print('\treturn ret;\n}', end='\n\n', file=output)

            else:
                print('\n}', end='\n\n', file=output)

        self.writeEnd(output, functionList)
        # close the file
        output.close()
