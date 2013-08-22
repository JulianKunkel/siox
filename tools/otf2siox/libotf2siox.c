/**
 * @file libotf2siox.c
 * @author Alvaro Aguilera
 */

#include "libotf2siox.h"


int import_otf( const char * filename, const uint32_t maxfiles,
                const uint32_t ppn )
{
#ifndef NDEBUG
	printf( "\
Loading OTF trace: %s\n\
Maximum event files allowed: %d\n\
Processes per SIOX node: %d\n\
", filename, maxfiles, ppn );
#endif
	struct otf_data odata = ( const struct otf_data ) {
		0
	};

	open_otf( &odata, filename, maxfiles );
	count_definitions( &odata );
	close_otf( &odata );

#ifndef NDEBUG
	printf( "Counted definitions: processes %d, files %d, functions %d\n",
	        odata.proc_count, odata.file_count, odata.func_count );
#endif

	allocate_def_arrays( &odata );

	open_otf( &odata, filename, maxfiles );

	load_definitions( &odata );

	struct siox_data sdata = ( const struct siox_data ) {
		0
	};

	register_nodes( &odata, &sdata, ppn );
	process_events( &odata, &sdata );

	close_otf( &odata );

	checkout( &sdata );
	murica( &odata, &sdata );

	return 0;
}


void open_otf( struct otf_data * odata, const char * filename,
               const uint32_t maxfiles )
{
	if( !( odata->manager = OTF_FileManager_open( maxfiles ) ) ) {
		perror( "Error opening the OTF file manager:" );
		exit( ENOMANAGER );
	}
	if( !( odata->handlers = OTF_HandlerArray_open() ) ) {
		perror( "Error opening the OTF handlers:" );
		exit( ENOHANDLER );
	}
	if( !( odata->reader = OTF_Reader_open( filename, odata->manager ) ) ) {
		perror( "Error opening the OTF events:" );
		exit( ENOEVENTS );
	}
}


void close_otf( struct otf_data * odata )
{
	OTF_Reader_close( odata->reader );
	OTF_HandlerArray_close( odata->handlers );
	OTF_FileManager_close( odata->manager );
}


void count_definitions( struct otf_data * odata )
{
	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) inc_proc_count,
	                             OTF_DEFPROCESS_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFPROCESS_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) inc_function_count,
	                             OTF_DEFFUNCTION_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFFUNCTION_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) inc_file_count,
	                             OTF_DEFFILE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFFILE_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) inc_keyvalue_count,
	                             OTF_DEFKEYVALUE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFKEYVALUE_RECORD );

	if( OTF_Reader_readDefinitions( odata->reader, odata->handlers )
	    == OTF_READ_ERROR ) {
		perror( "Error counting OTF definitions:" );
		exit( EIO );
	}

}


int inc_proc_count( struct otf_data * odata, uint32_t stream, uint32_t process,
                    const char * name, uint32_t parent, OTF_KeyValueList * kvlist )
{
	odata->proc_count++;
	return OTF_RETURN_OK;
}


int inc_file_count( struct otf_data * odata, uint32_t stream, uint32_t token,
                    const char * name, uint32_t funcGroup,
                    OTF_KeyValueList * kvlist )
{
	odata->file_count++;
	return OTF_RETURN_OK;
}


int inc_function_count( struct otf_data * odata, uint32_t stream, uint32_t func,
                        const char * name, uint32_t funcGroup, uint32_t source,
                        OTF_KeyValueList * kvlist )
{
	odata->func_count++;
	return OTF_RETURN_OK;
}


int inc_keyvalue_count( struct otf_data * odata, uint32_t stream, uint32_t key,
                        OTF_Type type, const char * name, const char * desc,
                        OTF_KeyValueList * kvlist )
{
	odata->kv_count++;
	return OTF_RETURN_OK;

}


void allocate_def_arrays( struct otf_data * d )
{
	d->processes = malloc( d->proc_count * sizeof( uint32_t ) );
	if( !d->processes ) {
		perror( "Error allocating memory for process array:" );
		exit( ENOMEM );
	}

	d->functions = malloc( d->func_count * sizeof( struct finfo ) );
	if( !d->functions ) {
		perror( "Error allocating memory for function array:" );
		exit( ENOMEM );
	}

	d->files = malloc( d->file_count * sizeof( char * ) );
	if( !d->files ) {
		perror( "Error allocating memory for file array:" );
		exit( ENOMEM );
	}

	d->keyvals = malloc( d->kv_count * sizeof( struct keyvalue ) );
	if( !d->keyvals ) {
		perror( "Error allocating memory for key-value array:" );
		exit( ENOMEM );
	}
}


void load_definitions( struct otf_data * odata )
{
	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_version,
	                             OTF_DEFVERSION_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFVERSION_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_creator,
	                             OTF_DEFCREATOR_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFCREATOR_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_resolution,
	                             OTF_DEFTIMERRESOLUTION_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFTIMERRESOLUTION_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_range,
	                             OTF_DEFTIMERANGE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFTIMERANGE_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_process,
	                             OTF_DEFPROCESS_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFPROCESS_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_file,
	                             OTF_DEFFILE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFFILE_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_function,
	                             OTF_DEFFUNCTION_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFFUNCTION_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_function_group,
	                             OTF_DEFFUNCTIONGROUP_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFFUNCTIONGROUP_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) def_keyvalue,
	                             OTF_DEFKEYVALUE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, odata,
	                                     OTF_DEFKEYVALUE_RECORD );

	if( OTF_Reader_readDefinitions( odata->reader, odata->handlers )
	    == OTF_READ_ERROR ) {
		perror( "Error loading OTF definitions:" );
		exit( EIO );
	}

}


int def_version( struct otf_data * odata, uint32_t stream, uint8_t major,
                 uint8_t minor, uint8_t sub, const char * string,
                 OTF_KeyValueList * kvlist )
{
	if( !( odata->version = ( char * ) malloc( strlen( string ) + 1 ) ) ) {
		perror( "Error allocating memory for version:" );
		exit( ENOMEM );
	}
	strcpy( odata->version, string );

#ifndef NDEBUG
	printf( "OTF version %s\n", odata->version );
#endif
	return OTF_RETURN_OK;
}


int def_creator( struct otf_data * odata, uint32_t stream, const char * creator,
                 OTF_KeyValueList * kvlist )
{
	if( !( odata->creator = ( char * ) malloc( strlen( creator ) + 1 ) ) ) {
		perror( "Error allocating memory for creator:" );
		exit( ENOMEM );
	}
	strcpy( odata->creator, creator );

#ifndef NDEBUG
	printf( "OTF creator %s\n", odata->creator );
#endif
	return OTF_RETURN_OK;
}


int def_resolution( struct otf_data * odata, uint32_t stream,
                    uint64_t ticksPerSecond, OTF_KeyValueList * kvlist )
{
	odata->resolution = ticksPerSecond;

#ifndef NDEBUG
	printf( "OTF timer resolution %ld\n", odata->resolution );
#endif
	return OTF_RETURN_OK;

}


int def_range( struct otf_data * odata, uint32_t stream, uint64_t mintime,
               uint64_t maxtime, OTF_KeyValueList * kvlist )
{
	odata->range_min = mintime;
	odata->range_max = maxtime;

#ifndef NDEBUG
	printf( "OTF timer range (%ld, %ld)\n", odata->range_min,
	        odata->range_max );
#endif
	return OTF_RETURN_OK;
}


int def_process( struct otf_data * odata, uint32_t stream, uint32_t process,
                 const char * name, uint32_t parent, OTF_KeyValueList * kvlist )
{
	static uint32_t idx = 0;
	odata->processes[idx] = process;

#ifndef NDEBUG
	printf( "Adding proccess[%d] = %s\n", idx, name );
#endif
	idx++;
	return OTF_RETURN_OK;
}


int def_file( struct otf_data * odata, uint32_t stream, uint32_t token,
              const char * name, uint32_t funcGroup, OTF_KeyValueList * kvlist )
{
	static uint32_t idx = 0;

	if( !( odata->files[idx] = ( char * ) malloc( strlen( name ) + 1 ) ) ) {
		perror( "Error allocating memory for file name:" );
		exit( ENOMEM );
	}
	strcpy( odata->files[idx], name );

#ifndef NDEBUG
	printf( "Adding file[%d] =  %s\n", idx, odata->files[idx] );
#endif
	idx++;

	return OTF_RETURN_OK;
}


int def_function( struct otf_data * odata, uint32_t stream, uint32_t func,
                  const char * name, uint32_t funcGroup, uint32_t source,
                  OTF_KeyValueList * kvlist )
{

	static uint32_t idx = 0;

	odata->functions[idx].name = ( char * ) malloc( strlen( name ) + 1 );

	if( !odata->functions ) {
		perror( "Error allocating memory for function name:" );
		exit( ENOMEM );
	}

	strcpy( odata->functions[idx].name, name );
	odata->functions[idx].group = funcGroup;

#ifndef NDEBUG
	printf( "Adding function[%d] = %s with group: %d\n", idx,
	        odata->functions[idx].name,
	        odata->functions[idx].group );
#endif

	idx++;

	return OTF_RETURN_OK;
}


int def_function_group( struct otf_data * odata, uint32_t stream,
                        uint32_t funcGroup, const char * name,
                        OTF_KeyValueList * kvlist )
{
	return OTF_RETURN_OK;
}


int def_keyvalue( struct otf_data * odata, uint32_t stream, uint32_t key,
                  OTF_Type type, const char * name, const char * desc,
                  OTF_KeyValueList * kvlist )
{
	static uint32_t idx = 0;

	odata->keyvals[idx].name = ( char * ) malloc( strlen( name ) + 1 );
	if( !odata->keyvals[idx].name ) {
		perror( "Error allocating memory for key-value name:" );
		exit( ENOMEM );
	}
	strcpy( odata->keyvals[idx].name, name );

	odata->keyvals[idx].desc = ( char * ) malloc( strlen( desc ) + 1 );
	if( !odata->keyvals[idx].desc ) {
		perror( "Error allocating memory for key-value description:" );
		exit( ENOMEM );
	}
	strcpy( odata->keyvals[idx].desc, desc );

	odata->keyvals[idx].type = type;

#ifndef NDEBUG
	printf( "Adding key[%d] = %s (%s)\n", idx,
	        odata->keyvals[idx].name, odata->keyvals[idx].desc );
#endif
	idx++;

	return OTF_RETURN_OK;
}


void register_nodes( struct otf_data * odata, struct siox_data * sdata,
                     const uint32_t ppn )
{
	sdata->ppn = ppn;
	sdata->node_count = odata->proc_count;
	sdata->nodes = ( siox_unid * )
	               malloc( sdata->node_count * sizeof( siox_unid ) );

	if( !sdata->nodes ) {
		perror( "Error allocating memory for SIOX nodes:" );
		exit( ENOMEM );
	}

	char * hwid = 0, *iid = 0;
	const char * swid = "VampirTrace I/O";
	uint32_t i = 0, node = 0;

	for( i = 0; i < odata->proc_count; i++ ) {

		if( iid )
			free( iid );

		if( !( iid = ( char * ) malloc( 30 * sizeof( char ) ) ) ) {
			perror( "Error allocating memory for iid:" );
			exit( ENOMEM );
		}
		sprintf( iid, "Process %d", i % sdata->ppn );

		if( i % sdata->ppn == 0 ) {

			node++;

			free( hwid );
			if( !( hwid = ( char * ) malloc( 30 * sizeof( char ) ) ) ) {
				perror( "Error allocating memory for hwid:" );
				exit( ENOMEM );
			}
			sprintf( hwid, "otf2siox fictive node %d", node );

			sdata->nodes[i] = siox_register_node( hwid, swid, iid );
			if( !sdata->nodes[i] ) {
				perror( "Error registering SIOX node:" );
				exit( ENOMEM );
			}

		} else {

			sdata->nodes[i] = siox_register_node( hwid, swid, iid );

		}

		/**
		 * @todo The way of registering sub-nodes or edges should
		 * probably be given more thought and improved in the next
		 * iteration of the SIOX API.
		 */

		siox_register_edge( sdata->nodes[i], "MPI" );
		siox_register_edge( sdata->nodes[i], "LIBC" );
		siox_register_edge( sdata->nodes[i], "POSIX" );

	}

}


void process_events( struct otf_data * odata, struct siox_data * sdata )
{

#ifndef NDEBUG
	puts( "\nProcessing OTF events..." );
#endif
	struct userdata u = {odata, sdata, 0};
	init_op_stacks( &u.stack, odata->proc_count );

	/* Set the handler functions for the OTF events. */
	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) event_enter,
	                             OTF_ENTER_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, &u,
	                                     OTF_ENTER_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) event_begin_io,
	                             OTF_BEGINFILEOP_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, &u,
	                                     OTF_BEGINFILEOP_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) event_end_io,
	                             OTF_ENDFILEOP_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, &u,
	                                     OTF_ENDFILEOP_RECORD );

	OTF_HandlerArray_setHandler( odata->handlers,
	                             ( OTF_FunctionPointer * ) event_leave,
	                             OTF_LEAVE_RECORD );
	OTF_HandlerArray_setFirstHandlerArg( odata->handlers, &u,
	                                     OTF_LEAVE_RECORD );

	if( OTF_Reader_readEvents( odata->reader, odata->handlers )
	    == OTF_READ_ERROR ) {
		perror( "Error loading OTF events:" );
		exit( EIO );
	}

}


int event_enter( struct userdata * u, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList * kvlist )
{
	struct finfo * fi = malloc( sizeof( struct finfo ) );

	if( get_finfo( u->odata, u->odata->functions[function - 1].name, &fi )
	    != 0 ) {
		free( fi );
		return OTF_RETURN_OK;
	}

#ifndef NDEBUG
	printf( "Event Enter function #%d => %s\n", function, fi->name );
#endif

	struct ioop * op = malloc( sizeof( struct ioop ) );
	if( !op ) {
		perror( "Error allocating memory for new operation:" );
		exit( ENOMEM );
	}

	op->finfo        = fi;
	op->source       = source;
	op->process      = process;
	op->finfo->group = u->odata->functions[function - 1].group;
	op->unid         = u->siox->nodes[process - 1];

	push_op( &u->stack[process - 1], op );

	return OTF_RETURN_OK;
}


int event_begin_io( struct userdata * u, uint64_t time, uint32_t process,
                    uint64_t matchingId, uint32_t scltoken,
                    OTF_KeyValueList * kvlist )
{
#ifndef NDEBUG
	puts( "\tBegin I/O" );
#endif
	struct ioop * op = u->stack[process - 1]->op;
	op->time = time;
	op->aid  = siox_start_activity( u->siox->nodes[process - 1],
	                                op->finfo->name );

	return OTF_RETURN_OK;
}


int event_end_io( struct userdata * u, uint64_t time, uint32_t process,
                  uint32_t fileid, uint64_t matchingId, uint64_t handleid,
                  uint32_t operation, uint64_t bytes, uint32_t scltoken,
                  OTF_KeyValueList * kvlist )
{
#ifndef NDEBUG
	puts( "\tEvent End I/O" );
#endif
	struct ioop * op = u->stack[process - 1]->op;

	siox_stop_activity( op->aid );

	op->operation = operation & OTF_FILEOP_BITS;
	op->flags     = operation & OTF_IOFLAGS_BITS;
	op->fileid    = fileid - 1;
	op->handleid  = handleid;
	op->bytes     = bytes;
	op->duration  = ( time - op->time ) / u->odata->resolution;

	/** @todo The key-value pairs should be sent to SIOX using the next
	 * version of SIOX' API.
	 */
//  uint32_t key, len, type, i = 0, j = 0;
//  uint8_t *byte_array = NULL;
//  OTF_Value value;
//
//  /* These key-value pairs should be added into SIOX as soon as there
//     is an API function making it possible. */
//  while (!OTF_KeyValueList_getKeyByIndex(kvlist, i, &key)) {
//
//      type = OTF_KeyValueList_getTypeForKey(kvlist, key);
//
//      switch (type) {
//      case OTF_CHAR:
//          OTF_KeyValueList_getChar(kvlist, key,
//                       &(value.otf_char));
//          printf("\t\t\tKeyval >%s< = %c\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_char);
//          break;
//      case OTF_UINT8:
//          OTF_KeyValueList_getUint8(kvlist, key,
//                        &(value.otf_uint8));
//          printf("\t\t\tKeyval >%s< = %hu\n",
//                 u->odata->keyvals[key].name,
//                 (unsigned short) value.otf_uint8);
//          break;
//      case OTF_INT8:
//          OTF_KeyValueList_getInt8(kvlist, key,
//                       &(value.otf_int8));
//          printf("\t\t\tKeyval >%s< = %hd\n",
//                 u->odata->keyvals[key].name,
//                 (short) value.otf_int8);
//          break;
//      case OTF_UINT16:
//          OTF_KeyValueList_getUint16(kvlist, key,
//                         &(value.otf_uint16));
//          printf("\t\t\tKeyval >%s< = %hu\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_uint16);
//          break;
//      case OTF_INT16:
//          OTF_KeyValueList_getInt16(kvlist, key,
//                        &(value.otf_int16));
//          printf("\t\t\tKeyval >%s< = %hd\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_int16);
//          break;
//      case OTF_UINT32:
//          OTF_KeyValueList_getUint32(kvlist, key,
//                         &(value.otf_uint32));
//          printf("\t\t\tKeyval: >%s< = %u\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_uint32);
//          break;
//      case OTF_INT32:
//          OTF_KeyValueList_getInt32(kvlist, key,
//                        &(value.otf_int32));
//          printf("\t\t\tKeyval: >%s< = %d\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_int32);
//          break;
//      case OTF_UINT64:
//          OTF_KeyValueList_getUint64(kvlist, key,
//                         &(value.otf_uint64));
//          printf("\t\t\tKeyval >%s< = %llu\n",
//                 u->odata->keyvals[key].name,
//                 (unsigned long long int) value.otf_uint64);
//          break;
//      case OTF_INT64:
//          OTF_KeyValueList_getInt64(kvlist, key,
//                        &(value.otf_int64));
//          printf("\t\t\tKeyval >%s< = %lld\n",
//                 u->odata->keyvals[key].name,
//                 (long long int) value.otf_int64);
//          break;
//      case OTF_FLOAT:
//          OTF_KeyValueList_getFloat(kvlist, key,
//                        &(value.otf_float));
//          printf("\t\t\tKey >%s< = %.3f\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_float);
//          break;
//      case OTF_DOUBLE:
//          OTF_KeyValueList_getDouble(kvlist, key,
//                         &(value.otf_double));
//          printf("\t\t\tKeyval >%s< = %.3f\n",
//                 u->odata->keyvals[key].name,
//                 value.otf_double);
//          break;
//      case OTF_BYTE_ARRAY:
//          OTF_KeyValueList_getArrayLength(kvlist, key, &len);
//
//          byte_array = (uint8_t*) malloc(len * sizeof(uint8_t));
//          OTF_KeyValueList_getByteArray(kvlist, key, byte_array,
//                            &len);
//          printf("\t\t\tKeyval >%s< = ",
//                 u->odata->keyvals[key].name);
//
//          for (j = 0; j < len; j++) {
//              printf("%02hX", (unsigned short) byte_array[j]);
//          }
//
//          free(byte_array);
//          break;
//      }
//
//      i++;
//  }
//
	return OTF_RETURN_OK;
}


int event_leave( struct userdata * u, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList * kvlist )
{
	struct ioop * op;

	if( pop_op( &u->stack[process - 1], &op ) )
		return OTF_RETURN_OK;

#ifndef NDEBUG
	printf( "Event Leave\n\n" );
#endif

	switch( op->finfo->family ) {
		case CLOS_F:
			report_close( u, op );
			break;
		case LOCK_F:
			report_lock( u, op );
			break;
		case OPEN_F:
			report_open( u, op );
			break;
		case READ_F:
			report_read( u, op );
			break;
		case SEEK_F:
			report_seek( u, op );
			break;
		case STAT_F:
			report_stat( u, op );
			break;
		case WRIT_F:
			report_write( u, op );
			break;
		default:
			printf( "Unimplmented function family ID %d.\n",
			        op->finfo->family );
	}

	siox_end_activity( op->aid );

	free( op->finfo );
	free( op );

	return OTF_RETURN_OK;
}


void report_open( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fname_t;
	static siox_dtid fid_t;
	static siox_dmid idnamemap;
	static siox_mid time_m;

	if( !checked_in ) {
		fname_t = siox_register_datatype( "OTF-File-Name",
		                                  SIOX_STORAGE_STRING );
		fid_t   = siox_register_datatype( "OTF-File-ID",
		                                  SIOX_STORAGE_64_BIT_INTEGER );
		idnamemap = siox_register_descriptor_map( op->unid, fname_t,
		            fid_t );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	char * fname = u->odata->files[op->fileid];

	siox_create_descriptor( op->aid, fname_t, &fname );
	siox_send_descriptor( op->aid, "LIBC", fname_t, &fname );

	char fid_s[10];
	sprintf( fid_s, "%d", ( uint32_t ) op->fileid );
	siox_map_descriptor( op->aid, idnamemap, &fname, &fid_s );

	// The duration time is reported extra because no I/O action is actually
	// executed and the time between start and stop activity is meaningless.
	siox_report_activity( op->aid, fname_t, &fname, time_m, &op->duration,
	                      "Duration of the file open call." );

	siox_release_descriptor( op->aid, fname_t, &fname );

}


void report_write( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid bytes_m, time_m;

	if( !checked_in ) {
		fid_t   = siox_register_datatype( "OTF-File-ID",
		                                  SIOX_STORAGE_64_BIT_INTEGER );
		bytes_m = siox_register_metric( "Bytes Written", "",
		                                SIOX_UNIT_BYTES,
		                                SIOX_STORAGE_64_BIT_INTEGER,
		                                SIOX_SCOPE_SUM );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_aid write_a = op->aid;
	siox_report_activity( write_a, fid_t, &op->fileid, bytes_m, &op->bytes,
	                      "Bytes written." );
	siox_report_activity( write_a, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of write call." );

}


void report_read( struct userdata * u,  struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid bytes_m, time_m;

	if( !checked_in ) {
		fid_t   = siox_register_datatype( "OTF-File-ID",
		                                  SIOX_STORAGE_64_BIT_INTEGER );
		bytes_m = siox_register_metric( "Bytes Read", "",
		                                SIOX_UNIT_BYTES,
		                                SIOX_STORAGE_64_BIT_INTEGER,
		                                SIOX_SCOPE_SUM );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, bytes_m, &op->bytes,
	                      "Bytes read." );
	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of read call." );

}


void report_seek( struct userdata * u,  struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file seek call." );

}


void report_close( struct userdata * u,  struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file close call." );
}


void report_lock( struct userdata * u,  struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {

		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file lock call." );

}


void report_stat( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file stat call." );

}


void report_del( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file delete call." );

}


void report_cntl( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of the file stat call." );

}


void report_other( struct userdata * u, struct ioop * op )
{
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;

	if( !checked_in ) {
		fid_t = siox_register_datatype( "OTF-File-ID",
		                                SIOX_STORAGE_64_BIT_INTEGER );
		time_m = siox_register_metric( "Completion Time", "",
		                               SIOX_UNIT_SECONDS,
		                               SIOX_STORAGE_DOUBLE,
		                               SIOX_SCOPE_MAXIMUM );
		checked_in = 1;
	}

	siox_report_activity( op->aid, fid_t, &op->fileid, time_m, &op->duration,
	                      "Duration of unclassified file call." );

}


void init_op_stacks( struct op_stack ** *s, uint32_t n )
{
	if( !( *s = malloc( n * sizeof( struct op_stack * ) ) ) ) {
		perror( "Error allocating memory for stack:" );
		exit( ENOMEM );
	}

	int i;
	for( i = 0; i < n; ( *s )[i++] = NULL )
		;

}


void push_op( struct op_stack ** s, struct ioop * op )
{
	struct op_stack * node;

	if( !( node = malloc( sizeof( struct op_stack ) ) ) ) {
		perror( "Error allocating memory for new stack node:" );
		exit( ENOMEM );
	}

	node->op = op;
	node->next = *s;
	*s = node;
}


int pop_op( struct op_stack ** s, struct ioop ** op )
{
	if( !*s )
		return -1;


	struct op_stack * node;
	node = *s;
	*s = ( *s )->next;

	*op = node->op;

	return 0;
}


int get_finfo( struct otf_data * odata, const char * fname, struct finfo ** f )
{
	int i;

	static struct finfo io_functions[] = IO_FUNCTIONS;

	for( i = 0; i < IO_FUNCTIONS_COUNT; i++ )

		if( strcmp( io_functions[i].name, fname ) == 0 ) {

			( *f )->name   = strdup( fname );
			( *f )->family = io_functions[i].family;
			( *f )->type   = io_functions[i].type;
			( *f )->layer  = io_functions[i].layer;

			return 0;
		}

	return -1;

}


void checkout( struct siox_data * sdata )
{
	uint32_t i;

	for( i = 0; i < sdata->node_count;
	     siox_unregister_node( sdata->nodes[i++] ) )
		;
}


void murica( struct otf_data * odata, struct siox_data * sdata )
{
	uint32_t i;

	for( i = 0; i < odata->func_count;
	     free( odata->functions[i++].name ) )
		;
	free( odata->functions );

	for( i = 0; i < odata->file_count;
	     free( odata->files[i++] ) )
		;
	free( odata->files );

	for( i = 0; i < odata->kv_count; i++ ) {
		free( odata->keyvals[i].name );
		free( odata->keyvals[i].desc );
	}
	free( odata->keyvals );

	free( odata->creator );
	free( odata->version );
	free( odata->processes );
	free( sdata->nodes );
}
