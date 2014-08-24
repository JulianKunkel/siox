#include <mpi.h>
#include "siox-datatypes-helper-mpi.h"

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>
#include <stdio.h>

// Could be auto-generated. As well as inverse translator.
static inline unsigned translateMPIOpenFlagsToSIOX( unsigned flags )
{
        return  ( ( flags & MPI_MODE_RDONLY ) > 0 ? SIOX_MPI_MODE_RDONLY : 0 ) |  ( ( flags & MPI_MODE_RDWR ) > 0 ? SIOX_MPI_MODE_RDWR : 0 ) |  ( ( flags & MPI_MODE_WRONLY ) > 0 ? SIOX_MPI_MODE_WRONLY : 0 ) |  ( ( flags & MPI_MODE_CREATE ) > 0 ? SIOX_MPI_MODE_CREATE : 0 ) |  ( ( flags & MPI_MODE_EXCL ) > 0 ? SIOX_MPI_MODE_EXCL : 0 ) |  ( ( flags & MPI_MODE_DELETE_ON_CLOSE ) > 0 ? SIOX_MPI_MODE_DELETE_ON_CLOSE : 0 ) |  ( ( flags & MPI_MODE_UNIQUE_OPEN ) > 0 ? SIOX_MPI_MODE_UNIQUE_OPEN : 0 ) |  ( ( flags & MPI_MODE_SEQUENTIAL ) > 0 ? SIOX_MPI_MODE_SEQUENTIAL : 0 ) |  ( ( flags & MPI_MODE_APPEND ) > 0 ? SIOX_MPI_MODE_APPEND : 0 );
}

static inline int translateMPIThreadLevelToSIOX( int flags )
{
        return  ( ( flags & MPI_THREAD_SINGLE ) > 0 ? SIOX_MPI_THREAD_SINGLE : 0 ) |  ( ( flags & MPI_THREAD_FUNNELED ) > 0 ? SIOX_MPI_THREAD_FUNNELED : 0 ) |  ( ( flags & MPI_THREAD_SERIALIZED ) > 0 ? SIOX_MPI_THREAD_SERIALIZED : 0 ) |  ( ( flags & MPI_THREAD_MULTIPLE ) > 0 ? SIOX_MPI_THREAD_MULTIPLE : 0 );
}

static inline enum MPIDatatype translateElementaryMPIDatatype( MPI_Datatype type ){
 	if (type == MPI_CHAR ) return SIOX_MPI_TYPE_CHAR;
 	if (type == MPI_SHORT ) return SIOX_MPI_TYPE_SHORT;
 	if (type == MPI_INT ) return SIOX_MPI_TYPE_INT;
 	if (type == MPI_LONG ) return SIOX_MPI_TYPE_LONG;
 	if (type == MPI_UNSIGNED_CHAR ) return SIOX_MPI_TYPE_UNSIGNED_CHAR;
 	if (type == MPI_UNSIGNED_SHORT ) return SIOX_MPI_TYPE_UNSIGNED_SHORT;
 	if (type == MPI_UNSIGNED_LONG ) return SIOX_MPI_TYPE_UNSIGNED_LONG;
 	if (type == MPI_UNSIGNED ) return SIOX_MPI_TYPE_UNSIGNED;
 	if (type == MPI_FLOAT ) return SIOX_MPI_TYPE_FLOAT;
 	if (type == MPI_DOUBLE ) return SIOX_MPI_TYPE_DOUBLE;
 	if (type == MPI_LONG_DOUBLE ) return SIOX_MPI_TYPE_LONG_DOUBLE;
 	if (type == MPI_BYTE ) return SIOX_MPI_TYPE_BYTE;
 	if (type == MPI_PACKED ) return SIOX_MPI_TYPE_PACKED;
 	return SIOX_MPI_TYPE_UNKNOWN;
}

// this routine is taken from PIOSIM, mpi-names.c, author Paul Mueller
static const char * getCombinerName(int combiner)
{
	if(combiner == MPI_COMBINER_NAMED)
		return "NAMED";
	else if(combiner == MPI_COMBINER_DUP)
		return "DUP";
	else if(combiner == MPI_COMBINER_CONTIGUOUS)
		return "CONTIGUOUS";
	else if(combiner == MPI_COMBINER_VECTOR)
		return "VECTOR";
	else if(combiner == MPI_COMBINER_HVECTOR_INTEGER)
		return "HVECTOR_INTEGER";
	else if(combiner == MPI_COMBINER_HVECTOR)
		return "HVECTOR";
	else if(combiner == MPI_COMBINER_INDEXED)
		return "INDEXED";
	else if(combiner == MPI_COMBINER_HINDEXED_INTEGER)
		return "HINDEXED_INTEGER";
	else if(combiner == MPI_COMBINER_HINDEXED)
		return "HINDEXED";
	else if(combiner == MPI_COMBINER_INDEXED_BLOCK)
		return "INDEXED_BLOCK";
	else if(combiner == MPI_COMBINER_STRUCT_INTEGER)
		return "STRUCT_INTEGER";
	else if(combiner == MPI_COMBINER_STRUCT)
		return "STRUCT";
	else if(combiner == MPI_COMBINER_SUBARRAY)
		return "SUBARRAY";
	else if(combiner == MPI_COMBINER_DARRAY)
		return "DARRAY";
	else if(combiner == MPI_COMBINER_F90_REAL)
		return "F90_REAL";
	else if(combiner == MPI_COMBINER_F90_COMPLEX)
		return "F90_COMPLEX";
	else if(combiner == MPI_COMBINER_F90_INTEGER)
		return "F90_INTEGER";
	else if(combiner == MPI_COMBINER_RESIZED)
		return "RESIZED";
	else
	{
		printf("[MPI] %s: unknown combiner constant requested: %d\n", __FUNCTION__, combiner);
		return "UNKNOWN";
	}
}

static const char * getOrderConstantName(int constant)
{
	if(constant == MPI_ORDER_FORTRAN)
		return "FORTRAN";
	else if(constant == MPI_ORDER_C)
		return "C";
	else
	{
		printf("[MPI] %s: unknown constant constant requested: %d\n", __FUNCTION__, constant);
		return "UNKNOWN";
	}
}

static const char * getDistributeConstantName(int constant)
{
	if(constant == MPI_DISTRIBUTE_BLOCK)
		return "BLOCK";
	else if(constant == MPI_DISTRIBUTE_CYCLIC)
		return "CYCLIC";
	else if(constant == MPI_DISTRIBUTE_NONE)
		return "NONE";
	else if(constant == MPI_DISTRIBUTE_DFLT_DARG)
		return "DFLT_DARG";
	else
	{
		printf("[MPI] %s: unknown constant constant requested: %d\n", __FUNCTION__, constant);
		return "UNKNOWN";
	}
}

#define APPEND_STR(myStr, count) if(*length < *pos + count ){ *str = (char*) realloc(*str, 2 * *length);  *length = 2 * *length; } *pos += sprintf(*str + *pos, "%s", myStr);

#define APPEND_STRSEP(myStr, count) if(*length < *pos + count + 1 ){ *str = (char*) realloc(*str, 2 * *length); *length = 2 * *length; } *pos = (((*str)[*pos-1] == ',') ? *pos - 1 : *pos); *pos += sprintf(*str + *pos, ";%s", myStr);

#define APPEND_INT(myInt) if(*length < *pos + 10 ){ *str = (char*)  realloc(*str, 2 * *length); *length = 2 * *length; } *pos += sprintf(*str + *pos, "%lld,", (long long int) myInt);

#define APPEND_COMMA *pos += sprintf(*str + *pos, ",");


static inline void datatypeToString(char ** str, int * pos, int * length, MPI_Datatype type){
	// this routine is partly taken from PIOSIM, write_info.c, author Paul Mueller
	int max_integers,	max_addresses, max_datatypes,	combiner;

	PMPI_Type_get_envelope(type, &max_integers, &max_addresses, &max_datatypes, &combiner);

	APPEND_STR(getCombinerName(combiner), 20);
	APPEND_STR("(", 1);
	if(combiner == MPI_COMBINER_NAMED)	// cannot call get_contents on this
	{
		int resultlen;
		char typename[MPI_MAX_OBJECT_NAME];
		PMPI_Type_get_name(type, typename, &resultlen);
		APPEND_STR(typename, 20);
		APPEND_STR(")", 1);

	}	else	{
		int *integers = malloc(sizeof(int) * max_integers);
		MPI_Aint *addresses = malloc(sizeof(MPI_Aint) * max_addresses);
		MPI_Datatype *datatypes = malloc(sizeof(MPI_Datatype) * max_datatypes);
		PMPI_Type_get_contents(type, max_integers, max_addresses, max_datatypes, integers, addresses, datatypes);


		if(combiner == MPI_COMBINER_DARRAY){
			// this is not nice, but we need to log the names of the constantes
			// used by mpi_create_darray, not the numerical values
			assert(max_integers > 2);

			APPEND_STR("SIZE=", 5);
			APPEND_INT(integers[0]);
			APPEND_STRSEP("RANK=", 5);
			APPEND_INT(integers[1]);
			APPEND_STRSEP("DIM=", 5);
			APPEND_INT(integers[2]);

			int dims = integers[2];

			APPEND_STRSEP("GS=", 5);
			int i = 3;
			for(; i < dims + 3; ++i){
				APPEND_INT(integers[i]);
			}

			APPEND_STRSEP("D=", 5);
			for(; i < dims * 2 + 3; ++i){
				APPEND_STR( getDistributeConstantName(integers[i]), 20 );
				APPEND_COMMA
			}
			APPEND_STRSEP("A=", 5);
			for(; i < dims * 3 + 3; ++i){
				APPEND_INT(integers[i]);
			}
			APPEND_STRSEP("P=", 5);
			for(; i < dims * 3 + 3; ++i){
				APPEND_INT(integers[i]);
			}

			APPEND_STR(getOrderConstantName(integers[max_integers - 1]), 20);
		}else if(combiner == MPI_COMBINER_SUBARRAY){
			for(int i = 0; i < max_integers - 1; ++i)
			{
				APPEND_INT(integers[i]);
			}

			APPEND_STR(getOrderConstantName(integers[max_integers - 1]), 20);
		}else if(combiner == MPI_COMBINER_STRUCT){
			APPEND_STR("BLOCKS=", 10);
			APPEND_INT(integers[0]);

			APPEND_STRSEP("BL=", 4);
			for(int i = 1; i < max_integers; ++i)
			{
				APPEND_INT(integers[i]);
			}
		}else{
			for(int i = 0; i < max_integers; ++i)
			{
				APPEND_INT(integers[i]);
			}
		}

		APPEND_STRSEP("ADDR=", 8);

		for(int i = 0; i < max_addresses; ++i)
		{
			APPEND_INT(addresses[i]);
		}

		APPEND_STRSEP("TYPES=", 10);
		for(int i = 0; i < max_datatypes; ++i)
		{
			datatypeToString(str, pos, length, datatypes[i]);
			APPEND_COMMA
		}
		*pos = *pos -1;

		APPEND_STR(")", 1);

		free(integers);
		free(addresses);
		free(datatypes);
	}
}


// This function detects how a mpi datatype is composed and records this information into the given (string) attribute.
static inline void recordDatatype(siox_activity * sioxActivity, siox_attribute * attribute, MPI_Datatype type){
	// we assume this size suffices
	int max_length = 2048;
	int pos = 0;
	char * type_str = malloc(max_length);

	datatypeToString(& type_str, & pos, & max_length, type);

	siox_activity_set_attribute( sioxActivity, attribute, type_str );

	free(type_str);
}

/*
 The list of known MPI Hints and the corresponding SIOX attribute.
 */
struct known_hint_t{
	char * name;
	siox_attribute * attribute;
};

// Three different types are supported, int32, int64 and strings.
static struct known_hint_t * knownHintValueInt32;

static struct known_hint_t * knownHintValueInt64;

static struct known_hint_t * knownHintValueStr;

static struct known_hint_t * optimizeHints;

void initMPIHelper(){
	{
	// support the known hints to the data structures, since the values are only available after init, it can't be done statically.
	struct known_hint_t tmp [] = {
		{"mpiio_concurrency",  infoConcurrency},
		{"mpiio_coll_contiguous",  infoCollContiguous},
		{NULL, NULL}};
	knownHintValueInt32 = malloc(sizeof(tmp));
	memcpy(knownHintValueInt32, tmp, sizeof(tmp));
	}
	{
	struct known_hint_t tmp [] = {
		{"cb_buffer_size" ,  infoBuffSize},
		{"romio_cb_read",  infoROMIOCollReadEnabled},
		{"romio_cb_write",  infoROMIOCollWriteEnabled},
		{NULL, NULL}};
	optimizeHints = malloc(sizeof(tmp));
	memcpy(optimizeHints, tmp, sizeof(tmp));		
	}
	{
	struct known_hint_t tmp [] = {
		{"romio_cb_read",  infoROMIOCollReadEnabled},
		{"romio_cb_write",  infoROMIOCollWriteEnabled},
		{NULL, NULL}};	
	knownHintValueStr = malloc(sizeof(tmp));
	memcpy(knownHintValueStr, tmp, sizeof(tmp));		
	}
	{
	struct known_hint_t tmp [] = {
		{"cb_buffer_size" ,  infoBuffSize},
		{"ind_rd_buffer_size" ,  infoReadBuffSize},
		{"ind_wr_buffer_size", infoWriteBuffSize},
		{"coll_read_bufsize",  infoCollReadBuffSize},
		{"coll_write_bufsize",  infoCollWriteBuffSize},
		{NULL, NULL}};
	knownHintValueInt64 = malloc(sizeof(tmp));
	memcpy(knownHintValueInt64, tmp, sizeof(tmp));		
	}
}

static inline void setOptimalHints(struct known_hint_t * cur, MPI_Info info, siox_activity * activity){
	char value[MPI_MAX_INFO_VAL];	
	for( ; cur->name != NULL; cur ++){
		int ret = siox_suggest_optimal_value_for_str(global_component, cur->attribute, activity, value, MPI_MAX_INFO_VAL );

		if ( ret ){
			PMPI_Info_set( info, cur->name, value );
		}
	}
}


static inline MPI_Info setOptimalParametersForOpen(MPI_Info user_info, siox_activity * activity){
	int ret;

	if (user_info == MPI_INFO_NULL){		
		PMPI_Info_create( & user_info );
	}
	if (! knownHintValueInt64){
		return user_info;
	}	

	// try to set the known hints
	struct known_hint_t * cur;

	setOptimalHints(knownHintValueInt32, user_info, activity);
	setOptimalHints(knownHintValueInt64, user_info, activity);
	setOptimalHints(knownHintValueStr, user_info, activity);

	return user_info;
}

static inline int setOptimalInfo(MPI_Info write, MPI_Info read){
	char value[MPI_MAX_INFO_VAL];
	int setHint = 0;

	struct known_hint_t * cur;

	for( cur = optimizeHints; cur->name != NULL; cur ++){
		int isDefined;

		if ( read != MPI_INFO_NULL ){
			// int ret =
			PMPI_Info_get(read, cur->name, MPI_MAX_INFO_VAL, value, & isDefined);
			//printf( "GET %s %s %d\n", cur->name, value, isDefined );

			if ( isDefined ){
				continue;
			}
		}
		if( siox_suggest_optimal_value_str( global_component, cur->attribute, value, MPI_MAX_INFO_VAL ) ){
			PMPI_Info_set( write, cur->name, value );
			setHint = 1;
		}
	}

	return setHint;
}

static inline void setFileInfo(MPI_File fh, MPI_Info user_info){
	MPI_Info newInfo;
	int setHint;
	if ( user_info == MPI_INFO_NULL ){
		PMPI_Info_create( & newInfo );
		setHint = setOptimalInfo( newInfo, MPI_INFO_NULL );
	}else{
		setHint = setOptimalInfo( user_info, user_info );
	}

	if(setHint){
		if ( user_info == MPI_INFO_NULL ){
			PMPI_File_set_info( fh, newInfo );
			PMPI_Info_free(& newInfo);
		}else{
			PMPI_File_set_info( fh, user_info );
		}
	}
}

/*
	Record the recognized default info key/value pairs.
	Additionally, record all hints in a big string separated by "||" (hopefully no string contains ||).
	Note that this string may contain not recognized hints.
 */
static inline void recordFileInfo(siox_activity * sioxActivity, MPI_File fh){
	MPI_Info info = MPI_INFO_NULL;
	PMPI_File_get_info( fh, & info );
	if (info == MPI_INFO_NULL){
		return;
	}

	int number_of_keys = 0;
	PMPI_Info_get_nkeys(info, & number_of_keys);

	// printf("[MPI] hint count: %d\n", number_of_keys);

	// iterate over all hints, determine their size.
	unsigned stringLength = -1;
	for(int i=0; i < number_of_keys; i++){
		char key[MPI_MAX_INFO_KEY];
		char value[MPI_MAX_INFO_VAL];
		int isDefined;
		int ret;

		PMPI_Info_get_nthkey(info, i, key);
		ret = PMPI_Info_get(info, key, MPI_MAX_INFO_VAL, value, & isDefined);

		if ( ! isDefined || ret != MPI_SUCCESS ){
			continue;
		}
		stringLength += strlen(key) + strlen(value) + 4;

		// printf("[MPI] Hint: \"%s\", \"%s\"\n", key, value);
	}

	if	(stringLength > 0){
		// store all hints into one string
		char * allHints = malloc(stringLength + 2);
		char * curPos = allHints;

		for(int i=0; i < number_of_keys; i++){
			char key[MPI_MAX_INFO_KEY];
			char value[MPI_MAX_INFO_VAL];
			int isDefined;
			int ret;

			PMPI_Info_get_nthkey(info, i, key);
			ret = PMPI_Info_get(info, key, MPI_MAX_INFO_VAL, value, & isDefined);

			if ( ! isDefined || ret != MPI_SUCCESS ){
				continue;
			}

			curPos += sprintf(curPos, "%s||%s||", key, value);
		}

		// 0 termination, strip the last two |
		curPos[-2] = 0;

		siox_activity_set_attribute( sioxActivity , infoString,  allHints);

		free(allHints);
	}

	// check for known hints and record them:
	struct known_hint_t * cur;

	for( cur = knownHintValueInt32; cur->name != NULL; cur ++){
		int isDefined, ret;
		char value[MPI_MAX_INFO_VAL];
		ret = PMPI_Info_get(info, cur->name, MPI_MAX_INFO_VAL, value, & isDefined);

		if ( ! isDefined || ret != MPI_SUCCESS ){
			continue;
		}
		int32_t val = (int32_t) atoi(value);
		siox_activity_set_attribute( sioxActivity, cur->attribute , & val );
	}

	for( cur = knownHintValueInt64; cur->name != NULL; cur ++){
		int isDefined, ret;
		char value[MPI_MAX_INFO_VAL];
		ret = PMPI_Info_get(info, cur->name, MPI_MAX_INFO_VAL, value, & isDefined);

		if ( ! isDefined || ret != MPI_SUCCESS ){
			continue;
		}
		int64_t val = (int64_t) atol(value);
		siox_activity_set_attribute( sioxActivity, cur->attribute , & val );
	}

	for( cur = knownHintValueStr; cur->name != NULL; cur ++){
		int isDefined, ret;
		char value[MPI_MAX_INFO_VAL];
		ret = PMPI_Info_get(info, cur->name, MPI_MAX_INFO_VAL, value, & isDefined);

		if ( ! isDefined || ret != MPI_SUCCESS ){
			continue;
		}
		siox_activity_set_attribute( sioxActivity, cur->attribute , value );
	}
}

/* generated using the shell script
	for I in   ; do echo -n " ( ( flags & $I ) > 0 ? SIOX_$I : 0 ) | " ; done ; echo
*/


static inline void recordProcessesGroup(siox_activity * sioxActivity, MPI_Group group){
 	MPI_Group world_group;
 	MPI_Comm_group(MPI_COMM_WORLD, & world_group);

 	// check if this is COMM_WORLD
 	int res;
	MPI_Group_compare(group, world_group, & res );
	if (res == MPI_IDENT){
		return;
	}

 	int size;
 	MPI_Group_size(group, & size);

 	int ranksIn[size];
 	int ranksCommWorld[size];
	
 	for( int i = 0; i < size; i++ ){
 		ranksIn[i] = i;
 	}

	MPI_Group_translate_ranks( group, size, ranksIn, world_group, ranksCommWorld);

	extern siox_attribute *commProcessesGroup;
	extern siox_attribute *commProcessesWorld;

	for( int i = 0; i < size; i++ ){
 		siox_activity_set_attribute( sioxActivity, commProcessesGroup, & ranksIn[i] );
		siox_activity_set_attribute( sioxActivity, commProcessesWorld, & ranksCommWorld[i] );
 	}
}


static inline void recordProcessesComm(siox_activity * sioxActivity, MPI_Comm comm){
	MPI_Group group;
 	MPI_Comm_group(comm, & group);
 	recordProcessesGroup(sioxActivity, group);
}

static inline uint64_t getCommHandle(MPI_Comm comm){
	if( comm == MPI_COMM_WORLD ){
		return SIOX_MPI_COMM_WORLD;
	}else if ( comm == MPI_COMM_SELF ){
		return SIOX_MPI_COMM_SELF;
	}

	// TODO make this func work nicely with configure
	if (sizeof(comm) == sizeof(int)){
		return (unsigned) comm;
	}else{
		return (uint64_t) comm;
	}
}

static inline uint64_t getDatatypeHandle(MPI_Datatype type){
	uint64_t val = translateElementaryMPIDatatype(type);
	if (val == SIOX_MPI_TYPE_UNKNOWN){
		if (sizeof(type) == sizeof(int)){
			return (unsigned) type;
		}else{
			return (uint64_t) type;
		}
	}
	return val;
}

static inline uint64_t getConstructedDatatypeHandle(MPI_Datatype type){
	if (sizeof(type) == sizeof(int)){
		return (unsigned) type;
	}else{
		return (uint64_t) type;
	}
}
