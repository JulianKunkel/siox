#include "libotf2siox.h"


int import_otf(const char *filename, const uint32_t maxfiles, 
	       const uint32_t ppn)
{
#ifndef NDEBUG
	printf("\
Begin processing OTF file...\n\
\tLoading OTF trace: %s\n\
\tMaximun event files allowed: %d\n\
\tProcesses per SIOX node: %d\n\
", filename, maxfiles, ppn);
#endif

	struct otf_defs definitions = (const struct otf_defs){0};
	count_definitions(filename, maxfiles, &definitions);

#ifndef NDEBUG
	printf("\tCounted definitions: processes %d, files %d, functions %d\n", 
	       definitions.proc_count, definitions.file_count,
	       definitions.func_count);
#endif

	allocate_def_arrays(&definitions);
	load_definitions(filename, maxfiles, &definitions);
	
	struct siox_data sdata = (const struct siox_data){0};
	register_nodes(&definitions, &sdata, ppn);
	
	process_events(filename, maxfiles, &definitions, &sdata);

	checkout(&sdata);
	
	murica(&definitions, &sdata);
	
	return 0;

}


void count_definitions(const char *filename, const uint32_t maxfiles, 
		       struct otf_defs *definitions)
{
	open_otf(filename, maxfiles);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) inc_proc_count,
				    OTF_DEFPROCESS_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFPROCESS_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) inc_function_count, 
				    OTF_DEFFUNCTION_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFFUNCTION_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) inc_file_count,
				    OTF_DEFFILE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFFILE_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) inc_keyvalue_count,
				    OTF_DEFKEYVALUE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFKEYVALUE_RECORD);

	if (OTF_Reader_readDefinitions(reader, handlers) == OTF_READ_ERROR) {
		perror("Error counting OTF definitions:");
		exit(EIO);
	}
	
	close_otf();

}


void open_otf(const char *filename, const uint32_t maxfiles)
{
	if (!(manager = OTF_FileManager_open(maxfiles))) {
		perror("Error opening the OTF file manager:");
		exit(ENOMANAGER);
	}
	if (!(handlers = OTF_HandlerArray_open())) {
		perror("Error opening the OTF handlers:");
		exit(ENOHANDLER);
	}
	if (!(reader = OTF_Reader_open(filename, manager))) {
		perror("Error opening the OTF events:");
		exit(ENOEVENTS);
	}
}


void close_otf()
{
	OTF_Reader_close(reader);
	OTF_HandlerArray_close(handlers);
	OTF_FileManager_close(manager);
}


int inc_proc_count(struct otf_defs *definitions, uint32_t stream, 
		   uint32_t process, const char *name, uint32_t parent, 
		   OTF_KeyValueList *kvlist) 
{
	definitions->proc_count++;
	return OTF_RETURN_OK;
}


int inc_file_count(struct otf_defs *definitions, uint32_t stream, 
		   uint32_t token, const char *name, uint32_t funcGroup, 
		   OTF_KeyValueList *kvlist) 
{
	definitions->file_count++;
	return OTF_RETURN_OK;
}


int inc_function_count(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t func, const char *name, uint32_t funcGroup, 
		       uint32_t source, OTF_KeyValueList *kvlist) 
{
	definitions->func_count++;
	return OTF_RETURN_OK;
}


int inc_keyvalue_count(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t key, OTF_Type type, const char *name, 
		       const char *desc, OTF_KeyValueList *kvlist)
{
	definitions->kv_count++;
	return OTF_RETURN_OK;

}


void allocate_def_arrays(struct otf_defs *d)
{
	d->processes = (uint32_t *) malloc(d->proc_count * sizeof(uint32_t));
	if (!d->processes) {
		perror("Error allocating memory for process array:");
		exit(ENOMEM);
	}

	d->functions = (struct finfo *) 
		       malloc(d->func_count * sizeof(struct finfo));
	if (!d->functions) {
		perror("Error allocating memory for function array:");
		exit(ENOMEM);
	}
	
	d->files = (char **) malloc(d->file_count * sizeof(char *));
	if (!d->files) {
		perror("Error allocating memory for file array:");
		exit(ENOMEM);
	}
	
	d->keyvals = (struct keyvalue *) 
		     malloc(d->kv_count * sizeof(struct keyvalue));
	if (!d->keyvals) {
		perror("Error allocating memory for key-value array:");
		exit(ENOMEM);
	}
}


void load_definitions(const char *filename, const uint32_t maxfiles, 
		      struct otf_defs *definitions)
{
	open_otf(filename, maxfiles);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_version,
				    OTF_DEFVERSION_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFVERSION_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_creator,
				    OTF_DEFCREATOR_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFCREATOR_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_resolution,
				    OTF_DEFTIMERRESOLUTION_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFTIMERRESOLUTION_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_range,
				    OTF_DEFTIMERANGE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFTIMERANGE_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_process, 
				    OTF_DEFPROCESS_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFPROCESS_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_file, 
				    OTF_DEFFILE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFFILE_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_function, 
				    OTF_DEFFUNCTION_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFFUNCTION_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_function_group, 
				    OTF_DEFFUNCTIONGROUP_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFFUNCTIONGROUP_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) def_keyvalue, 
				    OTF_DEFKEYVALUE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, definitions, 
					    OTF_DEFKEYVALUE_RECORD);


	if (OTF_Reader_readDefinitions(reader, handlers) == OTF_READ_ERROR) {
		perror("Error loading OTF definitions:");
		exit(EIO);
	}
	
	close_otf();

}


int def_version(struct otf_defs *definitions, uint32_t stream, uint8_t major,
		uint8_t minor, uint8_t sub, const char *string, 
		OTF_KeyValueList *kvlist)
{
	if (!(definitions->version = (char *) malloc(strlen(string) + 1))) {
		perror("Error allocating memory for version:");
		exit(ENOMEM);
	}
	strcpy(definitions->version, string);
	
#ifndef NDEBUG
	printf("\tOTF version %s\n", definitions->version);
#endif 
	return OTF_RETURN_OK;
}


int def_creator(struct otf_defs *definitions, uint32_t stream, 
		const char *creator, OTF_KeyValueList *kvlist)
{
	if (!(definitions->creator = (char *) malloc(strlen(creator) + 1))) {
		perror("Error allocating memory for creator:");
		exit(ENOMEM);
	}
	strcpy(definitions->creator, creator);

#ifndef NDEBUG
	printf("\tOTF creator %s\n", definitions->creator);
#endif 
	return OTF_RETURN_OK;
}


int def_resolution(struct otf_defs *definitions, uint32_t stream, 
		   uint64_t ticksPerSecond, OTF_KeyValueList *kvlist)
{
	definitions->resolution = ticksPerSecond;
	
#ifndef NDEBUG
	printf("\tOTF timer resolution %ld\n", definitions->resolution);
#endif 
	return OTF_RETURN_OK;
	
}


int def_range(struct otf_defs *definitions, uint32_t stream, uint64_t mintime,
	      uint64_t maxtime, OTF_KeyValueList *kvlist)
{
	definitions->range_min = mintime;
	definitions->range_max = maxtime;
	
#ifndef NDEBUG
	printf("\tOTF timer range (%ld, %ld)\n", definitions->range_min,
	       definitions->range_max);
#endif 
	return OTF_RETURN_OK;
}


int def_process(struct otf_defs *definitions, uint32_t stream, uint32_t process,
		const char *name, uint32_t parent, OTF_KeyValueList *kvlist)
{
	static uint32_t idx = 0;
	definitions->processes[idx] = process;
	
#ifndef NDEBUG
	printf("\tAdding proccess[%d] = %s\n", idx, name);
#endif 
	idx++;
	return OTF_RETURN_OK;
}


int def_file(struct otf_defs *definitions, uint32_t stream, uint32_t token, 
	     const char *name, uint32_t funcGroup, OTF_KeyValueList *kvlist)
{
	static uint32_t idx = 0;
	
	if (!(definitions->files[idx] = (char *) malloc(strlen(name) + 1))) {
		perror("Error allocating memory for file name:");
		exit(ENOMEM);
	}
	strcpy(definitions->files[idx], name);

#ifndef NDEBUG
	printf("\tAdding file[%d] =  %s\n", idx, definitions->files[idx]);
#endif
	idx++;
	
	return OTF_RETURN_OK;
}


int def_function(struct otf_defs *definitions, uint32_t stream, uint32_t func, 
		 const char *name, uint32_t funcGroup, uint32_t source, 
		 OTF_KeyValueList *kvlist) {

	static uint32_t idx = 0;
	
	definitions->functions[idx].name = (char *) malloc(strlen(name) + 1);
	
	if (!definitions->functions) {
		perror("Error allocating memory for function name:");
		exit(ENOMEM);
	}
	
	strcpy(definitions->functions[idx].name, name);
	definitions->functions[idx].group = funcGroup;
	
#ifndef NDEBUG
	printf("\tAdding function[%d] = %s with group: %d\n", idx,
	       definitions->functions[idx].name, 
	       definitions->functions[idx].group);
#endif
	
	idx++;

	return OTF_RETURN_OK;
}


int def_function_group(struct otf_defs *definitions, uint32_t stream, 
		       uint32_t funcGroup, const char *name, 
		       OTF_KeyValueList *kvlist)
{
	if (strcmp(name, "I/O") == 0 || strcmp(name, "LIBC-I/O") == 0) {
		definitions->io_group = funcGroup;
#ifndef NDEBUG
		printf("\tI/O function group \"%s\" found at #%d\n", 
		       name, funcGroup);
#endif
	}
	
	return OTF_RETURN_OK;
}


int def_keyvalue(struct otf_defs *definitions, uint32_t stream, uint32_t key,
		 OTF_Type type, const char *name, const char *desc, 
		 OTF_KeyValueList *kvlist) 
{
	static uint32_t idx = 0;

	definitions->keyvals[idx].name = (char *) malloc(strlen(name) + 1);
	if (!definitions->keyvals[idx].name) {
		perror("Error allocating memory for key-value name:");
		exit(ENOMEM);
	}
	strcpy(definitions->keyvals[idx].name, name);
	
	definitions->keyvals[idx].desc = (char *) malloc(strlen(desc) + 1);
	if (!definitions->keyvals[idx].desc) {
		perror("Error allocating memory for key-value description:");
		exit(ENOMEM);
	}
	strcpy(definitions->keyvals[idx].desc, desc);
	
	definitions->keyvals[idx].type = type;
	
#ifndef NDEBUG
	printf("\tAdding key[%d] = %s (%s)\n", idx, 
	       definitions->keyvals[idx].name, definitions->keyvals[idx].desc);
#endif
	idx++;
	
	return OTF_RETURN_OK;
}


void register_nodes(struct otf_defs *definitions, struct siox_data *sdata, 
		    const uint32_t ppn)
{
	sdata->ppn = ppn;
	sdata->node_count = definitions->proc_count;
	sdata->nodes = (siox_unid *) 
		       malloc(sdata->node_count * sizeof(siox_unid));
	if (!sdata->nodes) {
		perror("Error allocating memory for SIOX nodes:");
		exit(ENOMEM);
	}

	char *hwid = 0, *iid = 0;
	const char *swid = "VampirTrace I/O";
	uint32_t i = 0, node = 0;
	
	for (i = 0; i < definitions->proc_count; i++) {
		
		if (iid) 
			free(iid);
		
		if (!(iid = (char *) malloc(30 * sizeof(char)))) {
			perror("Error allocating memory for iid:");
			exit(ENOMEM);
		}
		sprintf(iid, "Process %d", i % sdata->ppn);

		if (i % sdata->ppn == 0) {
		
			node++;
			
			free(hwid);
			if (!(hwid = (char *) malloc(30 * sizeof(char)))) {
				perror("Error allocating memory for hwid:");
				exit(ENOMEM);
			}
			sprintf(hwid, "otf2siox fictive node %d", node);

			sdata->nodes[i] = siox_register_node(hwid, swid, iid);
			if (!sdata->nodes[i]) {
				perror("Error registering SIOX node:");
				exit(ENOMEM);
			}
			
		} else {
			
			sdata->nodes[i] = siox_register_node(hwid, swid, iid);
			
		}
		
		siox_register_edge(sdata->nodes[i], "MPI-I/O");
		siox_register_edge(sdata->nodes[i], "LIBC");
		
	}
	
}


void process_events(const char *filename, const uint32_t maxfiles, 
		    struct otf_defs *definitions, struct siox_data *sdata)
{
	
#ifndef NDEBUG
	puts("\nProcessing OTF events...");
#endif 
	open_otf(filename, maxfiles);

	struct userdata u = {definitions, sdata, 0};

	/* Set the handler functions for the OTF events. */
	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) event_enter, 
				    OTF_ENTER_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, &u, OTF_ENTER_RECORD);
	
	OTF_HandlerArray_setHandler(handlers, (OTF_FunctionPointer *) event_io, 
				    OTF_FILEOPERATION_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, &u, 
					    OTF_FILEOPERATION_RECORD);

	OTF_HandlerArray_setHandler(handlers, (OTF_FunctionPointer *) 
				    event_begin_io, OTF_BEGINFILEOP_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, &u, 
					    OTF_BEGINFILEOP_RECORD);

	OTF_HandlerArray_setHandler(handlers, (OTF_FunctionPointer *) 
				    event_end_io, OTF_ENDFILEOP_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, &u, 
					    OTF_ENDFILEOP_RECORD);

	OTF_HandlerArray_setHandler(handlers, 
				    (OTF_FunctionPointer *) event_leave, 
				    OTF_LEAVE_RECORD);
	OTF_HandlerArray_setFirstHandlerArg(handlers, &u, OTF_LEAVE_RECORD);

	if (OTF_Reader_readEvents(reader, handlers) == OTF_READ_ERROR) {
		perror("Error loading OTF events:");
		exit(EIO);
	}
	
	close_otf();

}


int event_enter(struct userdata *u, uint64_t time, uint32_t function, 
		uint32_t process, uint32_t source, OTF_KeyValueList *kvlist)
{
#ifndef NDEBUG
	printf("\tEntering function #%d => %s\n", function, 
	       u->definitions->functions[function-1].name);
#endif 
	struct ioop *op = (struct ioop *) malloc(sizeof(struct ioop));
	if (!op) {
		perror("Error allocating memory for new operation:");
		exit(ENOMEM);
	}
	
	if ((op->family = 
		io_family(u->definitions->functions[function-1].name))) {
		op->iocall   = 1;
		op->time     = time;
		op->function = function;
		op->group    = u->definitions->functions[function-1].group;
		op->process  = process;
		op->unid     = u->siox->nodes[process-1];
		op->source   = source;
	} 

	if (u->op) {
		free(u->op->fname);
		free(u->op);
	}

	u->op = op;
	
	return OTF_RETURN_OK;
}


int event_io(struct userdata *u, uint64_t time, uint32_t fileid, 
	     uint32_t process, uint64_t handleid, uint32_t operation, 
	     uint64_t bytes, uint64_t duration, uint32_t source, 
	     OTF_KeyValueList *kvlist)
{
#ifndef NDEBUG
	puts("\tEntering I/O event.");
#endif

	struct ioop *op = u->op;
	
	op->fileid    = fileid;
	op->handleid  = handleid;
	op->operation = operation & OTF_FILEOP_BITS;
	op->flags     = operation & OTF_IOFLAGS_BITS;
	op->bytes     = bytes;
	op->duration  = duration;

	switch (op->family) {
	case OPEN_ID:
		hdler_open(u);
		break;
	case WRITE_ID:
		hdler_write(u);
		break;
	case READ_ID:
		hdler_read(u);
		break;
	case SEEK_ID:
		hdler_seek(u);
		break;
	case CLOSE_ID:
		hdler_close(u);
		break;
	case LOCK_ID:
		hdler_lock(u);
		break;
	default:
		puts("Unimplmented function ID.");
	}
	
	return OTF_RETURN_OK;
}

int event_begin_io(struct userdata *u, uint64_t time, uint32_t process, 
		   uint64_t matchingId, uint32_t scltoken, 
		   OTF_KeyValueList *kvlist)
{
#ifndef NDEBUG
	puts("\t\tBeginning I/O");
#endif
	
	u->op->time = time;

	return OTF_RETURN_OK;
}


int event_end_io(struct userdata *u, uint64_t time, uint32_t process, 
		 uint32_t fileid, uint64_t matchingId, uint64_t handleid, 
		 uint32_t operation, uint64_t bytes, uint32_t scltoken, 
		 OTF_KeyValueList *kvlist)
{
#ifndef NDEBUG
	puts("\t\tEnding I/O");

	uint32_t key, len, type, i = 0, j = 0;
	uint8_t *byte_array = NULL;
	OTF_Value value;
	
	/* These key-value pairs should be added into SIOX as soon as there
	   is an API function making it possible. */
	while (!OTF_KeyValueList_getKeyByIndex(kvlist, i, &key)) {
		
		type = OTF_KeyValueList_getTypeForKey(kvlist, key);
		
		switch (type) {
		case OTF_CHAR:
			OTF_KeyValueList_getChar(kvlist, key, 
						 &(value.otf_char));
			printf("\t\t\tKeyval >%s< = %c\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_char);
			break;
		case OTF_UINT8:
			OTF_KeyValueList_getUint8(kvlist, key, 
						  &(value.otf_uint8));
			printf("\t\t\tKeyval >%s< = %hu\n", 
			       u->definitions->keyvals[key].name, 
			       (unsigned short) value.otf_uint8);
			break;
		case OTF_INT8:
			OTF_KeyValueList_getInt8(kvlist, key, 
						 &(value.otf_int8));
			printf("\t\t\tKeyval >%s< = %hd\n", 
			       u->definitions->keyvals[key].name, 
			       (short) value.otf_int8);
			break;
		case OTF_UINT16:
			OTF_KeyValueList_getUint16(kvlist, key, 
						   &(value.otf_uint16));
			printf("\t\t\tKeyval >%s< = %hu\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_uint16);
			break;
		case OTF_INT16:
			OTF_KeyValueList_getInt16(kvlist, key, 
						  &(value.otf_int16));
			printf("\t\t\tKeyval >%s< = %hd\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_int16);
			break;
		case OTF_UINT32:
			OTF_KeyValueList_getUint32(kvlist, key, 
						   &(value.otf_uint32));
			printf("\t\t\tKeyval: >%s< = %u\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_uint32);
			break;
		case OTF_INT32:
			OTF_KeyValueList_getInt32(kvlist, key, 
						  &(value.otf_int32));
			printf("\t\t\tKeyval: >%s< = %d\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_int32);
			break;
		case OTF_UINT64:
			OTF_KeyValueList_getUint64(kvlist, key, 
						   &(value.otf_uint64));
			printf("\t\t\tKeyval >%s< = %llu\n", 
			       u->definitions->keyvals[key].name, 
			       (unsigned long long int) value.otf_uint64);
			break;
		case OTF_INT64:
			OTF_KeyValueList_getInt64(kvlist, key, 
						  &(value.otf_int64));
			printf("\t\t\tKeyval >%s< = %lld\n", 
			       u->definitions->keyvals[key].name, 
			       (long long int) value.otf_int64);
			break;
		case OTF_FLOAT:
			OTF_KeyValueList_getFloat(kvlist, key, 
						  &(value.otf_float));
			printf("\t\t\tKey >%s< = %.3f\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_float);
			break;
		case OTF_DOUBLE:
			OTF_KeyValueList_getDouble(kvlist, key, 
						   &(value.otf_double));
			printf("\t\t\tKeyval >%s< = %.3f\n", 
			       u->definitions->keyvals[key].name, 
			       value.otf_double);
			break;
		case OTF_BYTE_ARRAY:
			OTF_KeyValueList_getArrayLength(kvlist, key, &len);
			
			byte_array = (uint8_t*) malloc(len * sizeof(uint8_t));
			OTF_KeyValueList_getByteArray(kvlist, key, byte_array, 
						      &len);
			printf("\t\t\tKeyval >%s< = ", 
			       u->definitions->keyvals[key].name);

			for (j = 0; j < len; j++) {
				printf("%02hX", (unsigned short) byte_array[j]);
			}

			free(byte_array);
			break;
		}
	  	  
		i++;
	}
#endif
	
	u->op->operation = operation & OTF_FILEOP_BITS;
	u->op->flags     = operation & OTF_IOFLAGS_BITS;
	u->op->process   = process;
	u->op->fileid    = fileid - 1;
	u->op->handleid  = handleid;
	u->op->bytes     = bytes;
	u->op->duration  = time - u->op->time;

	switch (u->op->family) {
	case CLOSE_ID:
		hdler_close(u);
		break;
	case LOCK_ID:
		hdler_lock(u);
		break;
	case OPEN_ID:
		hdler_open(u);
		break;
	case READ_ID:
		hdler_read(u);
		break;
	case SEEK_ID:
		hdler_seek(u);
		break;
	case STAT_ID:
		hdler_stat(u);
		break;
	case WRITE_ID:
		hdler_write(u);
		break;
	default:
		printf("Unimplmented function family ID %d.\n", u->op->family);
	}

	return OTF_RETURN_OK;
}


int event_leave(struct userdata *u, uint64_t time, uint32_t function, 
		uint32_t process, uint32_t source, OTF_KeyValueList *kvlist) 
{
#ifndef NDEBUG
	printf("\tLeaving function\n\n");
#endif 
	
	return OTF_RETURN_OK;
}


void hdler_open(struct userdata *u) 
{
#ifndef NDEBUG
	puts("================== PROCESSING >>open<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fname_t;
	static siox_dtid fid_t;
	static siox_dmid idnamemap;
	static siox_mid time_m;
	
	if (!checked_in) {
		
		fname_t = siox_register_datatype("OTF-File-Name", 
						 SIOX_STORAGE_STRING);
		fid_t   = siox_register_datatype("OTF-File-ID", 
						 SIOX_STORAGE_64_BIT_INTEGER);
		idnamemap = siox_register_descriptor_map(u->op->unid, fname_t, 
							 fid_t);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
		
	}
	
	char *fname = u->definitions->files[u->op->fileid];
	siox_aid open_a = siox_start_activity(u->op->unid, "Opening file");

	// The I/O would have taken place at this point.

	siox_stop_activity(open_a);

	siox_create_descriptor(open_a, fname_t, &fname);
 	siox_send_descriptor(open_a, "LIBC", fname_t, &fname);

	char fid_s[10];
	sprintf(fid_s, "%d", (uint32_t) u->op->fileid); 
	siox_map_descriptor(open_a, idnamemap, &fname, &fid_s); 
	
	// The duration time is reported extra because no I/O action is actually
	// executed and the time between start and stop activity is meaningless.
	siox_report_activity(open_a, fname_t, &fname, time_m, &u->op->duration, 
			     "Duration of the file open call.");

	siox_release_descriptor(open_a, fname_t, &fname);
	siox_end_activity(open_a);
	
#ifndef NDEBUG
	puts("==============================================================");
#endif

}


void hdler_write(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>write<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid bytes_m, time_m;
	
	if (!checked_in) {
		
		fid_t   = siox_register_datatype("OTF-File-ID", 
						 SIOX_STORAGE_64_BIT_INTEGER);
		bytes_m = siox_register_metric("Bytes Written", "", 
					       SIOX_UNIT_BYTES, 
					       SIOX_STORAGE_64_BIT_INTEGER, 
					       SIOX_SCOPE_SUM);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
	}
	
	siox_aid write_a = siox_start_activity(u->op->unid, "Writing data");
	
	// The I/O would have taken place at this point.

	siox_stop_activity(write_a);

	siox_report_activity(write_a, fid_t, &u->op->fileid, bytes_m, 
			     &u->op->bytes, "Bytes written.");
	siox_report_activity(write_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, "Duration of write call.");
	siox_end_activity(write_a);

#ifndef NDEBUG
	puts("==============================================================");
#endif
}


void hdler_read(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>read<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid bytes_m, time_m;
	
	if (!checked_in) {
		
		fid_t   = siox_register_datatype("OTF-File-ID", 
						 SIOX_STORAGE_64_BIT_INTEGER);
		bytes_m = siox_register_metric("Bytes Read", "", 
					       SIOX_UNIT_BYTES,
					       SIOX_STORAGE_64_BIT_INTEGER, 
					       SIOX_SCOPE_SUM);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
	}
	
	siox_aid read_a = siox_start_activity(u->op->unid, "Reading data");
	
	// The I/O would have taken place at this point.

	siox_stop_activity(read_a);

	siox_report_activity(read_a, fid_t, &u->op->fileid, bytes_m, 
			     &u->op->bytes, "Bytes read.");
	siox_report_activity(read_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, "Duration of read call.");
	siox_end_activity(read_a);

#ifndef NDEBUG
	puts("==============================================================");
#endif
}


void hdler_seek(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>seek<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;
	
	if (!checked_in) {
		
		fid_t = siox_register_datatype("OTF-File-ID", 
					       SIOX_STORAGE_64_BIT_INTEGER);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
	}
	
	siox_aid seek_a = siox_start_activity(u->op->unid, "Seeking data");
	
	// The I/O would have taken place at this point.

	siox_stop_activity(seek_a);

	siox_report_activity(seek_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, 
			     "Duration of the file seek call.");
 
	siox_end_activity(seek_a);
	
#ifndef NDEBUG
	puts("==============================================================");
#endif
}


void hdler_close(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>close<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;
	
	if (!checked_in) {
		
		fid_t = siox_register_datatype("OTF-File-ID", 
					       SIOX_STORAGE_64_BIT_INTEGER);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
		
	}
	
	siox_aid close_a = siox_start_activity(u->op->unid, "Closing file");
 
	// The I/O would have taken place at this point.
 
	siox_stop_activity(close_a);
	siox_report_activity(close_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, 
			     "Duration of the file close call.");
 
	siox_end_activity(close_a);
	
#ifndef NDEBUG
	puts("==============================================================");
#endif
}


void hdler_lock(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>lock<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;
	
	if (!checked_in) {
		
		fid_t = siox_register_datatype("OTF-File-ID", 
					       SIOX_STORAGE_64_BIT_INTEGER);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
		
	}
	
	siox_aid lock_a = siox_start_activity(u->op->unid, "Locking file");
 
	// The I/O would have taken place at this point.
 
	siox_stop_activity(lock_a);
	siox_report_activity(lock_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, 
			     "Duration of the file lock call.");
	siox_end_activity(lock_a);
	
#ifndef NDEBUG
	puts("==============================================================");
#endif
	
}


void hdler_stat(struct userdata *u)
{
#ifndef NDEBUG
	puts("================== PROCESSING >>stat<< CALL ==================");
#endif
	static int checked_in = 0;
	static siox_dtid fid_t;
	static siox_mid time_m;
	
	if (!checked_in) {
		
		fid_t = siox_register_datatype("OTF-File-ID", 
					       SIOX_STORAGE_64_BIT_INTEGER);
		time_m = siox_register_metric("Completion Time", "", 
					      SIOX_UNIT_SECONDS, 
					      SIOX_STORAGE_64_BIT_INTEGER, 
					      SIOX_SCOPE_MAXIMUM);
		checked_in = 1;
		
	}
	
	siox_aid stat_a = siox_start_activity(u->op->unid, "Stating file");
 
	// The I/O would have taken place at this point.
 
	siox_stop_activity(stat_a);
	siox_report_activity(stat_a, fid_t, &u->op->fileid, time_m, 
			     &u->op->duration, 
			     "Duration of the file stat call.");
	siox_end_activity(stat_a);
	
#ifndef NDEBUG
	puts("==============================================================");
#endif
	
}

uint32_t io_family(const char *member)
{
	int i;
	
	char *write_members[] = WRITE_MEMBERS;
	for (i = 0; i < WRITE_LEN; i++)
		if (strcmp(write_members[i], member) == 0)
 			return WRITE_ID;
	
	char *read_members[] = READ_MEMBERS;
	for (i = 0; i < READ_LEN; i++)
		if (strcmp(read_members[i], member) == 0)
			return READ_ID;

	char *seek_members[] = SEEK_MEMBERS;
	for (i = 0; i < SEEK_LEN; i++)
		if (strcmp(seek_members[i], member) == 0)
			return SEEK_ID;

	char *open_members[] = OPEN_MEMBERS;
	for (i = 0; i < OPEN_LEN; i++)
		if (strcmp(open_members[i], member) == 0)
			return OPEN_ID;
	
	char *close_members[] = CLOSE_MEMBERS;
	for (i = 0; i < CLOSE_LEN; i++)
		if (strcmp(close_members[i], member) == 0)
			return CLOSE_ID;

	char *lock_members[] = LOCK_MEMBERS;
	for (i = 0; i < LOCK_LEN; i++)
		if (strcmp(lock_members[i], member) == 0)
			return LOCK_ID;
		
	char *stat_members[] = STAT_MEMBERS;
	for (i = 0; i < STAT_LEN; i++)
		if (strcmp(stat_members[i], member) == 0)
			return STAT_ID;

	return 0;
	
}


inline uint32_t is_mpi(const char *str)
{
	return strncmp(str, "MPI_", 4) >= 0;
}


void checkout(struct siox_data *sdata)
{
	uint32_t i;
	
	for (i = 0; i < sdata->node_count; 
	     siox_unregister_node(sdata->nodes[i++])) 
		;
	
}


void murica(struct otf_defs *definitions, struct siox_data *sdata)
{
	uint32_t i;
	
	for (i = 0; i < definitions->func_count; 
	     free(definitions->functions[i++].name))
		;
	free(definitions->functions);
	
	for (i = 0; i < definitions->file_count; 
	     free(definitions->files[i++]))
		;
	free(definitions->files);
	
	for (i = 0; i < definitions->kv_count; i++) {
		free(definitions->keyvals[i].name);
		free(definitions->keyvals[i].desc);
	}
	free(definitions->keyvals);
	
	free(definitions->creator);
	free(definitions->version);
	free(definitions->processes);
	free(sdata->nodes);
}
