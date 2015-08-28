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
        


        output.close()
