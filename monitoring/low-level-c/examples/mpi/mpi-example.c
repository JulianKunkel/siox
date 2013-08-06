
/*
 Pseudocode for demonstrating how SIOX is used to determine optimization parameters --
 in this case MPI hints. It includes reporting/monitoring functionality to feed the SIOX analytics tool chain.
 */

static siox_unid unid;
// Hashtables, one to hold each descriptor type we use to map activities horizontally
// (in this case, only one as only the file handles serve this purpose)
static GHashTable ht_filehandles;


static void __init(){
  // Register node with SIOX
  // For this example, we use
  //  - the hostname as HardwareID,
  //  - "MPI" as SoftwareID and
  //  -  a concatenation of PID and TID as InstanceID:
  char* hwid[HOST_NAME_MAX];
  char* swid = "MPI";
  char* iid[20];
  gethostname(hwid, HOST_NAME_MAX);
  sprintf(iid, "%d-%d", getpid(), gettid());
  unid = siox_register_node(hwid, swid, iid);

  // Register data types to be used with SIOX
  // Bind them to variables to avoid further lookups whenever referenced throughout the library
  siox_dtid dt_filename = siox_register_datatype("MPI Filename", SIOX_STORAGE_STRING);
  siox_dtid dt_filehandle = siox_register_datatype("MPI Filehandle", SIOX_64_BIT_INTEGER);
  siox_dtid dt_mode = siox_register_datatype("MPI File Open Mode", SIOX_32_BIT_INTEGER);
  siox_dtid dt_cbsize = siox_register_datatype("MPI Collective Buffer Size", SIOX_32_BIT_INTEGER);
  siox_dtid dt_cbtowrite = siox_register_datatype("Bytes to Write", SIOX_32_BIT_INTEGER);
  siox_dtid dt_comm = siox_register_datatype("MPI Communicator", SIOX_RAW, length);

  // Tell SIOX which of the attributes just registered may serve as descriptors
  // in calls to or by our functions
  siox_register_datatype_as_descriptor(dt_filename);
  siox_register_datatype_as_descriptor(dt_filehandle);

  // Create hashtables to use in mapping of AIDs:
  ht_filehandles = g_hash_table_new(& g_int_hash, & g_int_equal);

  // Define parameters to be used in possible optimizations
  siox_oid oid_cbsize = siox_register_optimizer(dt_cbsize);

  // Define metrics
  siox_mid m_bwritten = siox_register_metric("Bytes Written", SIOX_UNIT_BYTES, SIOX_STORAGE_32_BIT_INTEGER, SIOX_SCOPE_ACTUAL);
}


/**
  This function just shows how optimizations to hints or other global optimization values should be requested from SIOX.
  The return type and signature depend on the relation of the optimization to real objects / storage systems and so on.
 */
static void hint_user(MPI_Info info, ...){
   int exists;
   char value[8];
   MPI_Info_get(info, "cbSize", 8, value, & exists);

  int setValue = 0;

  // For the start_, stop_ and end_activity functions, NULL will prompt SIOX to draw a current time stamp.
  siox_aid aid = siox_start_activity(unid, NULL, "hint_user");

   if (exists){ // cbSize is set by the user, just use it...
       setValue = value;
     // SIOX could verify this value, but we skip this for now.
   }else{
    // NORMALLY: use default values, probably set by environment variables or by a global variable within MPI...
    //  setValue = MPI_DEFAULT_CB_SIZE;
    // this code is replaced with code that uses SIOX for a better analysis:
    // Ask for suggestion, stating the parameter to be used.
    siox_suggest_optimization(unid, oid_cbsize, & setValue);
  }

     // get the current system status
     // uint64_t currentNICUtilization;
     // siox_get_counter_value(unid, counterID, &value);

  if (setValue > 0){
    // do optimization xy, i.e., send additional stuff to server, modify local variables...

    // report the optimization level to SIOX:
    siox_report_attribute(aid, dt_cbsize, & cbSize);
  }

  siox_stop_activity(aid, NULL);
  siox_end_activity(aid, NULL);
}


int MPI_File_open(MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh) {
  siox_aid aid = siox_start_activity(unid, NULL, "File_open");
  
  // Report our input/call parameters to SIOX for matching.
  siox_remote_call_receive(aid, dt_filename, &filename);
  siox_remote_call_receive(aid, dt_comm, &comm);
  siox_remote_call_receive(aid, dt_mode, &amode);

  int ret =  // Regular "File open" code goes here, check for return value
  
  hint_user(info, ...);

  siox_stop_activity(aid, NULL);

  // report error message if any
  if (mpi_error) 
    siox_report_error(aid, ret);

  // Remember descriptor origin for possible horizontal activity relations:
  // Filehandle is created here first, so any other function making use of it (such as read() or
  // write()) can be tied to our AID through it, right until a call to close(fh) will remove
  // the association from the hash table.
  g_hash_table_insert(ht_filehandles, & filehandle, & aid);

  siox_end_activity(aid, NULL);
}


int MPI_File_write(MPI_File fh, void *buffer, int count, MPI_Datatype datatype, MPI_Status *status){
  siox_aid aid = siox_start_activity(unid, NULL, "File_write");

  // Report our input/call parameters to SIOX for (vertical) causal linking
  siox_remote_call_receive(aid, dt_filehandle, &fh);

  // Report our (horizontal) causal link to the activity originally creating the descriptor fh
  // (in this case, probably an open() call)
  siox_aid* aid2_p = (siox_aid *) g_hash_table_lookup(ht_filehandles, & fh);
  if (aid2_p != NULL)
    siox_relate_activities(aid, *aid2_p);

  // Report that we are about to issue a remote call and the relevant parameters to it
  // This is actually only needed for remote calls or those with neighbour nodes not instrumented.
  // Parameters are the target's HWID, SWID and IID (if known, else NULL)
  siox_rcid rcid = siox_describe_remote_call_start(aid, NULL, "MPI-IO", NULL);
    // Tell SIOX what parameters we send to the callee that may identify our call
    siox_remote_call_attribute(rcid, dt_filehandle, & fh);
    // This can also be a derived value, such as total byte size to be written:
    int size;
    MPI_Type_size(datatype, & size);
    size *= count;
    siox_remote_call_attribute(rcid, dt_btowrite, & size);
  siox_describe_remote_call_end(rcid);

  ret = // Regular "File write" code goes here

  siox_stop_activity(aid, NULL);

  // report error message if any
  if (mpi_error) 
    siox_report_error(aid, ret);

  // Report amount of data written - this is equal to the bytes to write from above
  // (unless an error occurred)
  // The last parameter is a string with remarks or details or, as in this case, NULL.
  siox_report_activity(aid, m_bwritten, & size);

  siox_end_activity(aid, NULL);
}