from __future__ import print_function
import re

#
# @brief The output class (write a file to disk)
#
class Writer():
    
    #
    # @brief Write a header file
    #
    # @param functions A list of function-objects to write
    def headerFile(self, options, functionList):
        # open the output file for writing
        output = open(options.outputFile, 'w')

        # write all function headers
        throwaway = ["((^\s*)|(\s+))extern\s+.*\("]

        for function in functionList:
            for match in throwaway:
                if re.search(match, function.getDefinition()):
                    function.name = ''

            if function.type == '':
                continue

            elif function.name == '':
                continue

            else:
                print(function.getDefinition(), end=';\n', sep='', file=output)

        # close the file
        output.close()

    def writeHeaderBegin(self, output, functionList, includes, globalOnce, precompiler=[]):
        # write all needed includes        
        for p in precompiler:
            print('#' + p, end='\n', file=output)

        print("// global includes: ", file=output)
        for i in includes:
            i =  i if i.startswith("<") else "<" + i + ">"
            print('#include ', i, end='\n', file=output)
        print('\n', file=output)

        # write all global-Templates
        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('global') != '':
                    print(templ.output('global', functionVariables), file=output)
        print("", file=output)

        # write global once from template
        print(globalOnce, file=output)

        return output


    def writeEnd(self, output, functionList):
        # write all global-Templates
        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('global_end') != '':
                    print(templ.output('global_end', functionVariables), file=output)
        print("", file=output)


    def writeBefore(self, function, output, functionVariables):
            # write the before-template for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('before', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
            # write the beforeLast-template for this function
            for templ in reversed(function.usedTemplateList):
                outputString = templ.output('beforeLast', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
    
    def writeAfter(self, function, output, functionVariables):
            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('afterFirst', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
            for templ in function.usedTemplateList:
                outputString = templ.output('after', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
            for templ in reversed(function.usedTemplateList):
                outputString = templ.output('afterLast', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

            # write all after-templates for this function
            for templ in function.usedTemplateList:
                outputString = templ.output('cleanup', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
            for templ in reversed(function.usedTemplateList):
                outputString = templ.output('cleanupLast', functionVariables).strip()
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)

    def functionVariables(self, function, call=""):
      return {"FUNCTION_NAME" : function.name,
                                      "FUNCTION_CALL" : call}

