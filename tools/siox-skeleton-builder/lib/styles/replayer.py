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

        templateParameters["includes"].append("<feign.h>")
        templateParameters["includes"].append("\"datatypes.h\"")
        templateParameters["includes"].append("\"wrapper.h\"")
        # write all needed includes
        for match in templateParameters["includes"]:
            print('#include ', match, end='\n', file=output)
        print('\n', file=output)   

        print("""// provide some information about the plugin
////////////////////////////////////////////
Plugin plugin = {
	.name = "???-replayer",
	.version = NULL,
	.intents = FEIGN_REPLAYER,
};

int layer_id = 13;

Plugin * init() {
    // give a sign of life
	printf("Hello from %s\\n", plugin.name);

	// announce features and request further action
	return &plugin;
}

""", file=output)

        print("""Activity * replay(Activity * activity) 
{
	FEIGN_LOG(3,"replay");

	if ( activity->layer == layer_id ) {
		posix_activity * sub_activity = (posix_activity*)activity->data;

        // a simple way to produce action
		switch ( sub_activity->type ) {
""",file=output)

        for function in functionList:
            functionVariables = self.functionVariables(function)
            # write function signature
            print("\t"*3, "case POSIX_", function.name, ":", end='\n', sep='', file=output)
            
            print("\t"*4, "wrapped_", function.name, "(sub_activity->data);", end='\n', sep='', file=output)
            print("\t"*4, "break;", end='\n\n', sep='', file=output)


        print("""
            default:
                FEIGN_LOG(8,"unknown type");
        }
    } else {
        FEIGN_LOG(5, "!");
    }

    return NULL;
} // end replay()

""", file=output)
        

        #precompiler.insert(0, "define _GNU_SOURCE")
        #templateParameters["includes"].append("dlfcn.h")

        #self.writeHeaderBegin(output, functionList, templateParameters["includes"], templateParameters["globalOnce"], precompiler);

        #print("static char* dllib = RTLD_NEXT;", file=output)

        #for function in functionList:
        #    print(function.getDefinitionPointer(), file=output)
        #print("\tstatic int initialized_dlsym = 0;\n", file=output)

        #print("\nstatic void feignSymbolInit() {\ninitialized_dlsym = 1;", file=output)
        #for function in functionList:
        #    print(function.getDlsym(), file=output)

        #print("}", file=output)

        #print("\nstatic void feignInit() {\n", file=output)
        #print("\tif(initialized_dlsym == 0) sioxSymbolInit();", file=output)
        ## write all init-templates
        #for function in functionList:
        #    functionVariables = self.functionVariables(function)

        #    for templ in function.usedTemplateList:
        #        outputString = templ.output('init', functionVariables)
        #        if outputString != '':
        #            print('\t', outputString, end='\n', sep='', file=output)
        #for function in reversed(functionList):
        #    functionVariables = self.functionVariables(function)

        #    for templ in function.usedTemplateList:
        #        outputString = templ.output('initLast', functionVariables)
        #        if outputString != '':
        #            print('\t', outputString, end='\n', sep='', file=output)
        #print("}", file=output)

        #print("\nstatic void sioxFinal() {", file=output)
        ## write all final-functions
        #for function in functionList:
        #    functionVariables = self.functionVariables(function)
        #    for templ in function.usedTemplateList:
        #        outputString = templ.output('final', functionVariables)
        #        if outputString != '':
        #            print('\t', outputString, end='\n', sep='', file=output)

        #print("}", file=output)

        #for function in functionList:

        #    # a variable to save the return-value
        #    returnType = function.type

        #    if returnType != "void":
        #        functionCall = '\tret = ' + function.getCallPointer() + ';\n'
        #    else:
        #        functionCall = '\t' + function.getCallPointer() + ';\n'

        #    functionVariables = self.functionVariables(function, "if(initialized_dlsym == 0) sioxSymbolInit();\n" + functionCall + "\n")

        #    # write function signature
        #    print(function.getDefinition(), end='\n{\n', sep=' ',
        #          file=output)

        #    # look for va_lists because they need special treament
        #    if function.parameterList[-1].type == "...":
        #        print('\tva_list valist;', file=output)
        #        print(
        #            '\tva_start(valist, %s);' % function.parameterList[-2].name,
        #            file=output)
        #     #   print( '\t%s val = va_arg(valist, %s);' % (function.parameterList[-2].type, function.parameterList[-2].type), file=output)
        #        # set the name to args
        #        function.parameterList[-1].name = "val"

        #    if returnType != "void":
        #        print('\t', returnType, ' ret;', end='\n', sep='',
        #              file=output)

        #    self.writeBefore(function, output, functionVariables)

        #    # write the function call
        #    print(functionCall, file=output)

        #    self.writeAfter(function, output, functionVariables)

        #    # look for va_lists because they need special treament
        #    if function.parameterList[-1].type == "...":
        #        print("\tva_end(valist);", file=output)

        #    # write the return statement and close the function
        #    if returnType != "void":
        #        print('\treturn ret;\n}', end='\n\n', file=output)

        #    else:
        #        print('\n}', end='\n\n', file=output)

        # close the file
        output.close()
