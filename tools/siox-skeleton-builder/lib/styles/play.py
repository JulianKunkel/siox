from __future__ import print_function

import skeletonBuilder
import StringIO

class Style(skeletonBuilder.Writer):
    #
    # @brief Write a source file
    #
    # @param functions A list of function-objects to write
    def writeOutput(self, options, functionList, templateParameters, precompiler):
        # open the output file for writing
        output = open(options.outputFile, 'w')
        print("/**\n * GENERATED PLUGIN! - Modify carefully. May loose changes on regeneration. \n */", file=output)

        ###templateParameters["includes"].append("<feign.h>")
        ###templateParameters["includes"].append("\"datatypes.h\"")
        ###templateParameters["includes"].append("\"wrapper.h\"")


        # write all needed includes
        for match in templateParameters["includes"]:
            print('#include ', match, end='\n', file=output)


        #######################################################################
        # Global Variables
        #######################################################################
        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('player_global') != '':
                    outputString = templ.output('player_global', functionVariables)
                    print(outputString, file=output)


        #######################################################################
        # Ucaid Mapper
        #######################################################################
        print("""
void ReplayPlugin::findUcaidMapping() 
{
	std::stringstream ss;

	try {

		//virtual UniqueInterfaceID           lookup_interfaceID( const string & interface, const string & implementation ) const throw( NotFoundError )  = 0;
		UniqueInterfaceID uiid = sys_info->lookup_interfaceID("POSIX", "");

        // GENERATED""", file=output)




        for function in functionList:
            expansion_template = """
            // %s
            posix_%s = sys_info->lookup_activityID(uiid, "%s"); ss << "%s" << " <=> " << posix_%s << " <=> " << sys_info->lookup_activity_name( posix_%s ) << std::endl;"""

            expansion_template = expansion_template % (
                function.name,
                function.name,
                function.name,
                function.name,
                function.name,
                function.name,
               )
            print(expansion_template, file=output)

        print("""
        // GENERATED END

	} catch( NotFoundError & e ) {	
		cerr << "Find Ucaid Mapping: Interface not found!" << ss.str() << endl;
		//cerr << "Exception" << e << endl;
	}
}
        """, file=output)



        #######################################################################
        # Attribute Mapper
        #######################################################################
        print("""
void ReplayPlugin::findAttributeMapping() 
{
	std::stringstream ss;


	std::cout << "FindAttributeMapping: interface by name\n";

	//virtual const OntologyAttribute      lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) = 0; 

	try {
        // GENERATED""", file=output)

        
	#	oa_dataChar          = facade->lookup_attribute_by_name("POSIX"  , "data/character"         /*, SIOX_STORAGE_32_BIT_INTEGER  */ );
	#	oa_dataCount         = facade->lookup_attribute_by_name("POSIX"  , "data/count"             /*, SIOX_STORAGE_32_BIT_UINTEGER */ );
	#	oa_memoryAddress     = facade->lookup_attribute_by_name("POSIX"  , "data/MemoryAddress"     /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_filePointer       = facade->lookup_attribute_by_name("POSIX"  , "descriptor/FilePointer" /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_bytesToRead       = facade->lookup_attribute_by_name("POSIX"  , "quantity/BytesToRead"   /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_bytesToWrite      = facade->lookup_attribute_by_name("POSIX"  , "quantity/BytesToWrite"  /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_filePosition      = facade->lookup_attribute_by_name("POSIX"  , "file/position"          /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_fileExtent        = facade->lookup_attribute_by_name("POSIX"  , "file/extent"            /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_fileMemoryRegions = facade->lookup_attribute_by_name("POSIX"  , "quantity/memoryRegions" /*, SIOX_STORAGE_32_BIT_INTEGER  */ );
	#	oa_fileOpenFlags     = facade->lookup_attribute_by_name("POSIX"  , "hints/openFlags"        /*, SIOX_STORAGE_32_BIT_UINTEGER */ );
	#	oa_fileName          = facade->lookup_attribute_by_name("POSIX"  , "descriptor/filename"    /*, SIOX_STORAGE_STRING          */ );
	#	oa_fileSystem        = facade->lookup_attribute_by_name("Global" , "descriptor/filesystem"  /*, SIOX_STORAGE_32_BIT_UINTEGER */ );
	#	oa_fileHandle        = facade->lookup_attribute_by_name("POSIX"  , "descriptor/filehandle"  /*, SIOX_STORAGE_32_BIT_UINTEGER */ );
	#	oa_bytesWritten      = facade->lookup_attribute_by_name("POSIX"  , "quantity/BytesWritten"  /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_bytesRead         = facade->lookup_attribute_by_name("POSIX"  , "quantity/BytesRead"     /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_fileAdviseExtent  = facade->lookup_attribute_by_name("POSIX"  , "hint/advise-extent"     /*, SIOX_STORAGE_64_BIT_UINTEGER */ );
	#	oa_fileAdvise        = facade->lookup_attribute_by_name("POSIX"  , "hints/advise"           /*, SIOX_STORAGE_32_BIT_INTEGER  */ );
	#	oa_fileBufferSize    = facade->lookup_attribute_by_name("POSIX"  , "hints/bufferSize"       /*, SIOX_STORAGE_64_BIT_INTEGER  */ );
	#	oa_fileBufferMode    = facade->lookup_attribute_by_name("POSIX"  , "hints/bufferMode"       /*, SIOX_STORAGE_32_BIT_INTEGER  */ );




        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('player_attribute_mapper') != '':
                    print(templ.output('player_attribute_mapper', functionVariables), file=output)

        print("""
        // END GENERATED

	} catch( NotFoundError & e ) {	
		cerr << "Exception during attribute mapping: Attribute not found!" << ss.str() << endl;
		//cerr << "Exception" << e << endl;
	}
}
        """, file=output)


        #######################################################################
        # Replay function
        #######################################################################
        print("/** \n * Turn siox-activities into system/library activity. \n */", file=output)
        print("""void ReplayPlugin::replayActivity( std::shared_ptr<Activity> activity )
{
	stringstream str;

	printActivity(activity);

	try {
		char buff[40];
		siox_time_to_str( activity->time_start(), buff, false );


		activity->time_stop();
		activity->time_start();

		activity->aid();

		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( activity->ucaid() );

		sys_info->lookup_interface_name( uid );
		sys_info->lookup_interface_implementation( uid );


		// prepare some commonly accessed variables
		int ucaid = activity->ucaid();
""", end='\n', sep='', file=output)


        # generate handler for every call/function
        for function in functionList:
            functionVariables = self.functionVariables(function)
            # write function signature
    
            expansion_template = """if( ucaid == posix_%s ) { 
            // TODO
            %s
        } 
        else"""

            
            generated_body = StringIO.StringIO()

            # insert sections as specified by annotations
            #print("\t"*3, "// GENERATED FROM TEMPLATE\n", file=generated_body)
            #sections = ["feign_wrapper_before", "feign_wrapper_after"]
            ## insert the previous sections in that order
            #for sectionName in sections:
            #    for templ in function.usedTemplateList:
            #        outputString = templ.output(sectionName, functionVariables)
            #        if outputString != '':
            #            print('\t', outputString, end='\n', sep='', file=generated_body)
            #print("\t"*3, "// GENERATED FROM TEMPLATE END", file=generated_body)


            # expand template / insert 
            expansion_template = expansion_template % (
                    function.name,
                    generated_body.getvalue()
                )

            print( expansion_template, end='\n', sep='', file=output)


        # finish the incomplete else part
        print("""{
        	// not found!
			printf("unknown type - nothing to replay");
		}       


        cout << str.str() << endl;

	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
	}

	printActivity(activity);
	printf("DONE \n\n");

} // end of ReplayPlugin::replayActivity()
""", file=output)
        


        output.close()
