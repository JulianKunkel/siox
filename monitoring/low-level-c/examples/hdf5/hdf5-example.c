/**
 * @file hdf5-example.c
 *
 * Example for using the SIOX low-level interface with HDF5,
 * demonstrating how to instrument a client for SIOX.
 * This client will demonstrate the use of several activities,
 * each encapsulating different actions causally linked to one another.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2013-08-16
 *          GNU Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <hdf5.h>
#include <hdf5_hl.h>

#include <C/siox-ll.h>



/**
 * Report error to SIOX, close current activity and shut down component.
 * @param [in] error_code   The error code to return to SIOX;
 *       usually, you'd pass on whatever the lower layer reported.
 * @param [in] activity     A pointer to the current activity's identifier
 * @param [in] component    A pointer to the current component's identifier
 */
void report_and_abort( siox_activity_error error_code, siox_activity * activity, siox_component * component );


/**
 * Using SIOX at Client Level
 *
 * An example of a client utilising SIOX with the HDF5 High Level Interface.
 *
 * @return  EXIT_SUCCESS if all went well, otherwise EXIT_FAILURE.
 */
int
main(){

    /* Vars for SIOX */

    // Unique interface identifiers
    siox_unique_interface *     siox_our_ui;    // This component

    // This component's identifier
    siox_component *            siox_our_component;
    
    // Identifier for our component-specific activity types
    siox_component_activity *   siox_act_type_open;
    siox_component_activity *   siox_act_type_write;
    siox_component_activity *   siox_act_type_close;

    // Identifiers for the attributes of our activities
    siox_attribute *            siox_att_filename;      // HDF5 file name
    siox_attribute *            siox_att_fileid;        // HDF5 file id
    siox_attribute *            siox_att_bytes2write;   // Bytes to write
    siox_attribute *            siox_att_byteswritten;  // Bytes written

    // Identifiers for our actual activities
    siox_activity *             siox_act_open;         // The open activity
    siox_activity *             siox_act_write;         // The write activity
    siox_activity *             siox_act_close;         // The close activity

    // A collector for performance data
    long int                    bytes_written;

    // Variables holding our interface and implementation strings
    char*   swid_s = "SIOX-HDF5-Example";
    char*   impl_s = "V2";



    /* Vars for accessing HDF5 */
    char*       hdf5_file_name = "File.h5";
    hid_t       hdf5_file_id;
    hsize_t     dims[2]={2,3};
    int         data[6]={1,2,3,4,5,6};
    herr_t      hdf5_status;



    /*
     * Info Message
     * ============
     */
     printf( "\n"
             "SIOX HDF5 Example\n"
             "=================\n"
             "\n" );


    /*
     * Checking in with SIOX
     * =====================
     */

    // Look up our interface and implementation info
    siox_our_ui = siox_system_information_lookup_interface_id( swid_s, impl_s );

    // Register node itself
    // In place of the NULL, a string may be given as an instance identifier,
    // such as a network port or an MPI rank.
    siox_our_component = siox_component_register( siox_our_ui, "" );

    // Register the activity types (as seen from our layer, not the underlying HDF5!)
    // we will use. Here, our work consists only of a single activity, writing to a file.
    // In contrast, HDF5 will have to serve calls to open(), write() and close() for us.
    siox_act_type_write = siox_component_register_activity( siox_our_ui, "write" );

    // Register the attributes we will need
    siox_att_filename = siox_ontology_register_attribute( "filesystem", "filename", SIOX_STORAGE_STRING );
    siox_att_filename = siox_ontology_register_attribute( "filesystem", "fileid", SIOX_STORAGE_64_BIT_INTEGER );
    siox_att_bytes2write = siox_ontology_register_attribute_with_unit( "io",
                                                                       "data volume/to write",
                                                                       "Bytes",
                                                                       SIOX_STORAGE_64_BIT_UINTEGER );
    siox_att_byteswritten = siox_ontology_register_attribute_with_unit( "io",
                                                                        "data volume/written",
                                                                        "Bytes",
                                                                        SIOX_STORAGE_64_BIT_UINTEGER );
    /// @todo Mark filename and fileid (and b2w?) as descriptors



    /*
     * Opening a File for Writing
     * ==========================
     */
     

    // Notify SIOX that we are starting an activity of one of the types we previously declared.
    // For this example, we start one activity each for the open(), write() and close() calls.
    siox_act_open = siox_activity_start( siox_our_component, siox_act_type_open );

    // Report the call's attributes: The file name we write to.
    siox_activity_set_attribute( siox_act_open, siox_att_filename, hdf5_file_name );

    // The actual call to HDF5 to create a file with the name "File.h5", returning an
    // HDF5 file id
    hdf5_file_id = H5Fcreate ( hdf5_file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
    if (hdf5_file_id < 0)
        report_and_abort(hdf5_file_id, siox_act_write, siox_our_component);

    // The HDF5 create() call maps the file name provided to a file id that is used
    // in all future actions on the file. We inform SIOX of this fact by adding the
    // new file id as an additional attribute to the open() activity.
    siox_activity_set_attribute( siox_act_write, siox_att_fileid, &hdf5_file_id );

    // Tell SIOX the activity has ended.
    // As there are no performance metrics to report here, we can do without a call
    // to siox_stop_activity() to separate active and reporting phases.
    siox_activity_end( siox_act_open );



    /*
     * Writing into the File
     * =====================
     */


    // Start another activity which will cover the writing part
    siox_act_write = siox_activity_start( siox_our_component, siox_act_type_write );

    // This time, we also inform SIOX that our current acivity is causally related
    // to the one of the open() call, as we reuse the same file then prepared for us.
    siox_activity_link_to_parent( siox_act_write, siox_act_open );

    // Once again, prepare the call's attributes for SIOX.
    // This call is identified by the HDF5 file id, not the HDF5 file name.
    // By the association we reported to SIOX during the open() activity, we might be able
    // to link these actions together, even if the link to parent call above was missing.
    siox_activity_set_attribute( siox_act_write, siox_att_fileid, &hdf5_file_id );
    // This time, we also compute the total number of bytes to write,
    // an identifying attribute of the call that is not directly passed as a parameter.
    long int b2write = sizeof( data );
    siox_activity_set_attribute( siox_act_write, siox_att_bytes2write, &b2write );

    // The actual call to create the file and write the dataset to it
    hdf5_status = H5LTmake_dataset( hdf5_file_id, "/dset", 2, dims, H5T_NATIVE_INT, data );
    if (hdf5_status < 0)
        report_and_abort(hdf5_status, siox_act_write, siox_our_component);

    // Stop the writing activity; this will mark the end of the active phase and begin reporting
    siox_activity_stop( siox_act_write );

    /*
     * Collecting and Reporting Performance Data
     */

    // Collect data
    bytes_written = sizeof( data );

    // Report the data we collected. This could take place anytime between siox_start_activity()
    // and siox_activity_end() and happen more than once per activity.
    siox_activity_set_attribute( siox_act_write, siox_att_byteswritten, &bytes_written );

    /* Notify SIOX that all pertinent data has been sent and the write activity can be closed */
    siox_activity_end( siox_act_write );



    /*
     * Closing the File
     * ================
     */


    /* One more activity, this time for the close() call. Instead of being placed in one activity
       each, any or all calls could also be grouped together in activities, as program logic dictates.  */
    siox_act_close = siox_activity_start( siox_our_component, siox_act_type_close );

    /* This time, we also inform SIOX that our current acivity is causally related to the one of
       the open() call, as we reuse the same file then prepared for us. */
    siox_activity_link_to_parent( siox_act_close, siox_act_open );

    // The actual call to close the file
    hdf5_status = H5Fclose( hdf5_file_id );
    if( hdf5_status < 0 )
        report_and_abort(hdf5_status, siox_act_close, siox_our_component);

    // Now close the activity covering the close(), too
    siox_activity_end( siox_act_close );



    /*
     * Cleaning up
     * ===========
     */


    /* Unregister component from SIOX */
    siox_component_unregister( siox_our_component );


    exit( EXIT_SUCCESS );
}



void report_and_abort( siox_activity_error error_code, siox_activity * activity, siox_component * component )
{
    fprintf( stderr, "!!! Error accessing file via HDF5! !!!" );
    
    // Report to SIOX that the activity ended in an error and disconnect from SIOX.
    siox_activity_report_error( activity, error_code );
    siox_activity_end( activity );
    siox_component_unregister( component );
    exit( EXIT_FAILURE );
}
