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
        print("/** * GENERATED PLUGIN! - Modify carefully. May loose changes on regeneration. \n */", file=output)

        #######################################################################
        # Includes
        #######################################################################

        ###templateParameters["includes"].append("<feign.h>")
        ###templateParameters["includes"].append("\"datatypes.h\"")
        ###templateParameters["includes"].append("\"wrapper.h\"")

        # write all needed includes
        #for match in templateParameters["includes"]:
        #    print('#include ', match, end='', file=output)

        print("""
#include <regex>
#include <map>
#include <unordered_map>
#include <iostream>
#include <string>

#include <util/time.h>
#include <monitoring/ontology/Ontology.hpp>
#include "Replay.hpp"


// Replay related includes
#include "posix-ll-io-helper.h"
#include <C/siox-ll.h>			// TODO: implementation provided by siox-ll-subset.cpp


#define SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER int32
#define SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER uint32
#define SUB_CAST_SIOX_STORAGE_64_BIT_INTEGER int64
#define SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER uint64
#define SUB_CAST_SIOX_STORAGE_STRING str
        """, file=output)


        #######################################################################
        # Global Variables
        #######################################################################
        print("// ucaid mapping", file=output)
        for function in functionList:
            expansion_template = """static int posix_%s = -1;"""

            expansion_template = expansion_template % (
                function.name,
               )
            print(expansion_template, file=output)

        
        print("\n// attribute mapping", file=output)
        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('player_global') != '':
                    outputString = templ.output('player_global', functionVariables)
                    print(outputString, file=output)


        print("\n// attribute macros", file=output)
        for function in functionList:
            functionVariables = self.functionVariables(function)
            for templ in function.usedTemplateList:
                if templ.output('player_attribute_macros') != '':
                    print(templ.output('player_attribute_macros', functionVariables), file=output)


        #######################################################################
        # Prefix Code / Init and Helpers
        #######################################################################
        print("""

// Replay related globals
int issued = 3;	// 3 will be first issued by this replayer
std::map<int,int> fds;
std::map<long,FILE*> streams;
std::map<int,int> activityHashTable_int;
std::map<int,int> activityHashTable_network_int;

//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
static VariableDatatype convert_attribute( OntologyAttribute & oa, const void * value )
{
	AttributeValue v;

	switch( oa.storage_type ) {
		case( VariableDatatype::Type::UINT32 ):
			return *( ( uint32_t * ) value );
		case( VariableDatatype::Type::INT32 ): {
			return *( ( int32_t * ) value );
		}
		case( VariableDatatype::Type::UINT64 ):
			return *( ( uint64_t * ) value );
		case( VariableDatatype::Type::INT64 ): {
			return *( ( int64_t * ) value );
		}
		case( VariableDatatype::Type::FLOAT ): {
			return  *( ( float * ) value );
		}
		case( VariableDatatype::Type::DOUBLE ): {
			return  *( ( double * ) value );
		}
		case( VariableDatatype::Type::STRING ): {
			return ( char * ) value;
		}
		case( VariableDatatype::Type::INVALID ): {
			assert( 0 );
		}
	}
	return "";
}

static bool convert_attribute_back( OntologyAttribute & oa, const VariableDatatype & val, void * out_value ){
	switch( val.type() ) {
		case VariableDatatype::Type::INT32:
			*((int32_t*) out_value) = val.int32();
			return true;
		case VariableDatatype::Type::UINT32:
			*((uint32_t*) out_value) = val.uint32();
			return true;
		case VariableDatatype::Type::INT64:
			*((int64_t*) out_value) = val.int64();
			return true;
		case VariableDatatype::Type::UINT64:
			*((uint64_t*) out_value) = val.uint64();
			return true;
		case VariableDatatype::Type::FLOAT:
			*((float*) out_value) = val.flt();
			return true;
		case VariableDatatype::Type::DOUBLE:
			*((double*) out_value) = val.dbl();
			return true;
		case VariableDatatype::Type::STRING: {
			*(char**) out_value = strdup(val.str());
			return true;
		}
		case VariableDatatype::Type::INVALID:
		default:
			assert(0 && "tried to optimize for a VariableDatatype of invalid type");
			return false;
	}
}


// Replay related helpers
/**
 * Special helper function to be used by write and reads, which require 
 * buffers of certain size. The function guarentees to provide a address to
 * a buffer of sufficient size.
 */
static char * shared_byte_buffer(unsigned int size){
	static char * buffer = NULL;
	static size_t buffer_size = 0;

	if (buffer_size >= size){
		return buffer;
	}

	// allocate memory for data to write
	if ((buffer = (char *)realloc(buffer, size))) {
		size_t i;
		for (i=buffer_size; i < size; i++){
			buffer[i] = '0';
		}

		buffer_size = size;

		printf("Memory reallocated for %lld bytes\\n", (long long int) size);
		return buffer;
	}
	else {
		printf("Not enough memory available to allocate %lld bytes!\\n",  (long long int) size);
		exit(1);
	}
}


void dump_fds() {
	printf("dump_fds()");
	for (auto it=fds.begin(); it!=fds.end(); ++it) {
        std::cout << " fds: " << it->first << " => " << it->second << std::endl;
	}
	printf("dump_fds() end ");
}

void dump_streams() {
	printf("dump_streams()");
	for (auto it=streams.begin(); it!=streams.end(); ++it) {
        std::cout << " streams: " << it->first << " => " << it->second << std::endl;
	}
	printf("dump_streams() end ");
}







ComponentOptions* ReplayPlugin::AvailableOptions()
{
	return new ReplayPluginOptions{};
}

void ReplayPlugin::initPlugin(){
	assert(multiplexer);
	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&ReplayPlugin::notify), false);
	sys_info = facade->get_system_information();
	assert(sys_info);
	std::cout << "Time (HH:MM:SS.NANO)\\tDuration\\tID\\tComponent\\tActivity(Attributes)\\tParents = ReturnCode" << std::endl;

	findUcaidMapping();
	findAttributeMapping();
}

void ReplayPlugin::notify(const std::shared_ptr<Activity>& activity, int lost){
	ReplayPluginOptions& opts = getOptions<ReplayPluginOptions>();
	switch (opts.verbosity) {
		case 0:
			replayActivity( activity );
			break;
		case 1:
			activity->print();
			break;
		default:
			cout << "Warninig: invalid verbosity level: " << opts.verbosity << endl;
			replayActivity( activity );
	}
}


static inline void strtime( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );

	time_t timeIns = ts;
	struct tm * timeStruct = gmtime( & timeIns );

	char buff[200];
	strftime( buff, 199, "%F %T", timeStruct );

	s << buff;
	snprintf( buff, 20, ".%.10lld", ( long long int ) ns );
	s << buff;
}

static inline void strdelta( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );
	char buff[200];
	snprintf( buff, 20, "%lld.%.10lld", ( long long int ) ts, ( long long int ) ns );
	s << buff;
}


void ReplayPlugin::strattribute( const Attribute& attribute, std::stringstream& s ) throw( NotFoundError )
{
	OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
	s << oa.domain << "/" << oa.name << "=";
	if( attribute.value.type() == VariableDatatype::Type::STRING){
		s << '"' << attribute.value << '"' ;
	}else{
		s << attribute.value ;
	}
}


bool ReplayPlugin::strattribute_compare( const Attribute& attribute, const char* attributeName) throw( NotFoundError )
{
	std::stringstream s;
	std::stringstream debug_string;

	OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
	s << oa.domain << "/" << oa.name;

	//if( attribute.value.type() == VariableDatatype::Type::STRING){
	//	s << '"' << attribute.value << '"' ;
	//}else{
	//	s << attribute.value ;
	//}
	
	debug_string << "TR: strattribute_compare: s=" << s.str() << "";
	cout << debug_string.str();

	if ( s.str().compare(attributeName) == 0 ) {
		return true;
	} else {
		return false;
	}
}




const AttributeValue ReplayPlugin::getActivityAttributeValueByName(  std::shared_ptr<Activity> a, const char * domain, const char * name) throw( NotFoundError )
{
	try {
		
		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( a->ucaid() );

		auto oa = facade->lookup_attribute_by_name( domain, name );

		for( auto itr = a->attributeArray().begin() ; itr != a->attributeArray().end(); itr++ ) {
			if ( itr->id == oa.aID ){
				// activity has attribute, return value
				return itr->value;
			} else {
				// not found in this iteration
			}
		}

	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: (ommited by getActivityAttributeValueByName)" << endl;
	}
}


        """, file=output)









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


	std::cout << "FindAttributeMapping: interface by name";

	//virtual const OntologyAttribute      lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) = 0; 

	try {
        // GENERATED""", file=output)

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
        print("/**  * Turn siox-activities into system/library activity. \n */", file=output)
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
""", end='', sep='', file=output)


        # generate handler for every call/function
        for function in functionList:
            functionVariables = self.functionVariables(function)
            # write function signature
    
            expansion_template = """if( ucaid == posix_%s ) { 
            // TODO
            %s
        } 
        else """

            
            generated_body = StringIO.StringIO()

            # insert sections as specified by annotations
            print("\t"*3, "// GENERATED FROM TEMPLATE", file=generated_body)
            print('\n', '\t'*3, '/* */', file=generated_body)

            
            paramlist = []

            # setup variables
            for parameter in function.parameterList:
                if parameter.type != "..." and parameter.type != "void":
                    
                    nonconst_type = parameter.type.replace("const ", "")

                    print("\t", nonconst_type  , end=' ', sep='', file=generated_body) 
                    print(parameter.name, end=';\n', sep='', file=generated_body) 

                    paramlist.append(parameter.name)

            # expand templates
            sections = ['player']
            # insert the previous sections in that order
            for sectionName in sections:
                for templ in function.usedTemplateList:
                    outputString = templ.output(sectionName, functionVariables)
                    if outputString != '':
                        print('\t', outputString, end='', sep='', file=generated_body)



            print("""ret = %s(%s);""" % (function.name, ", ".join(paramlist)), file=generated_body);


            # expand templates
            sections = ['player_after']
            # insert the previous sections in that order
            for sectionName in sections:
                for templ in function.usedTemplateList:
                    outputString = templ.output(sectionName, functionVariables)
                    if outputString != '':
                        print('\t', outputString, end='', sep='', file=generated_body)


            print('\n', '\t'*3, '/* */', file=generated_body)
            print('\n', '\t'*3, '// GENERATED FROM TEMPLATE END', file=generated_body)


            # expand template / insert 
            expansion_template = expansion_template % (
                    function.name,
                    generated_body.getvalue()
                )

            print( expansion_template, end='', sep='', file=output)


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
	printf("DONE \\n");

} // end of ReplayPlugin::replayActivity()
        """, file=output)
    



        #######################################################################
        # Remaining functionality / not generated
        #######################################################################
        print("""
void ReplayPlugin::printActivity( std::shared_ptr<Activity> activity )
{
	stringstream str;
	try {
		char buff[40];
		siox_time_to_str( activity->time_start(), buff, false );


		str << "REPLAY ONLINE:" << " ";

		str << buff << " ";

		strdelta( activity->time_stop() - activity->time_start(), str );
//		if( printHostname )
//			str << " " << sys_info->lookup_node_hostname( a->aid().cid.pid.nid );

		str  << " " << activity->aid() << " ";

		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( activity->ucaid() );

		str << sys_info->lookup_interface_name( uid ) << " ";
		str << "interface:\"" << sys_info->lookup_interface_implementation( uid ) << "\" ";

		str << sys_info->lookup_activity_name( activity->ucaid() ) << "(";
		for( auto itr = activity->attributeArray().begin() ; itr != activity->attributeArray().end(); itr++ ) {
			if( itr != activity->attributeArray().begin() ) {
				str << ", ";
			}
			strattribute( *itr, str );
		}
		str << ")";
		str << " = " << activity->errorValue();

		if( activity->parentArray().begin() != activity->parentArray().end() ) {
			str << " ";
			for( auto itr = activity->parentArray().begin(); itr != activity->parentArray().end(); itr++ ) {
				if( itr != activity->parentArray().begin() ) {
					str << ", ";
				}
				str << *itr;
			}
		}

		cout << str.str() << endl;


	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
	}
}


extern "C" {
void* MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME ()
	{
		return new ReplayPlugin();
	}
}
        """, file=output);








        output.close()
