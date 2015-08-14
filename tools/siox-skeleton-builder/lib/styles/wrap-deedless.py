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

        print("// DEEDLESS!", file=output)

        precompiler.insert(0, "define _GNU_SOURCE")
        self.writeHeaderBegin(output, functionList, templateParameters["includes"], templateParameters["globalOnce"], precompiler);

        # write all function redefinitions
        for function in functionList:
            print(function.getDefinitionReal(), end=';\n', sep='', file=output)

        print("", file=output)

        print("static void sioxInit() {", file=output)
        # write all init-templates
        for function in functionList:
            functionVariables = self.functionVariables(function)

            for templ in function.usedTemplateList:
                outputString = templ.output('init', functionVariables)
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
        
        for function in reversed(functionList):
            functionVariables = self.functionVariables(function)

            for templ in function.usedTemplateList:
                outputString = templ.output('initLast', functionVariables)
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
        print("}", file=output)

        print("static void sioxFinal() {", file=output)
        # write all final-functions
        for templ in function.usedTemplateList:
            outputString = templ.output('finalOnce', functionVariables)
            if outputString != '':
                print('\t', outputString, end='\n', sep='', file=output)
        for function in functionList:
            functionVariables = self.functionVariables(function)

            for templ in function.usedTemplateList:
                outputString = templ.output('final', functionVariables)
                if outputString != '':
                    print('\t', outputString, end='\n', sep='', file=output)
        for templ in function.usedTemplateList:
            outputString = templ.output('finalOnceLast', functionVariables)
            if outputString != '':
                print('\t', outputString, end='\n', sep='', file=output)                    
        print("}", file=output)

        # write all functions-bodies
        for function in functionList:
            # a variable to save the return-value
            returnType = function.type

            # write the function call
            if returnType != "void":
                functionCall = '\tret = ' + function.getCallReal() + ';\n'
            else:
                functionCall = '\t' + function.getCallReal() + ';\n'

            outputString = templ.output('final', functionVariables)

            # write function signature
            print(function.getDefinitionWrap(), end='\n{\n', sep='', file=output)

            # look for va_lists because they need special treament
            if function.parameterList[-1].type == "...":
                if not function.rewriteCall:
                    print("function: " + function.getDefinition() + " needs the rewriteCall annotation since it supports va_args")
                    exit(1)

                print('\tva_list valist;', file=output)
                print('\tva_start(valist, %s);' % function.parameterList[-2].name,
                    file=output)
                #print( '\t%s val = va_arg(valist, %s);' % (function.parameterList[-2].type,  function.parameterList[-2].type), file=output)

                # set the name to args
                function.parameterList[-1].name = "val"


            if returnType != "void":
                print('\t', returnType, ' ret = 0;', end='\n', sep='',
                      file=output)

            functionVariables = self.functionVariables(function, functionCall)
            self.writeBefore(function, output, functionVariables)

            # write the function call
            if function.writeFunctionCall:
                print(functionCall, file=output)
            
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
        output.close()

        # generate gcc string for the user
        if options.wrapFile:
            output = open(options.wrapFile, 'w')
            gccHelper = '-Wl'

            for function in functionList:
                gccHelper = "%s,--wrap=%s" % (gccHelper, function.name)

            print(gccHelper, file=output)
            output.close()

