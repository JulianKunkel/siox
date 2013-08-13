/**
 * @file    mockupfs-example.c
 *
 * Example for an application using a mock-up file system layer,
 * both of them instrumented for SIOX.
 * This example is slightly more complex, using SIOX's mechanism
 * for remote calls when passing parameters to underlying layers.
 *
 * @authors Julian Kunkel & Michaela Zimmer
 * @date    2013-08-06
 *          GNU Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#include <C/siox-ll.h>

#include "mufs.h"


/**
 * Using SIOX
 *
 * An example of a client utilising a mock-up file system.
 *
 * @return  @c EXIT_FAILURE upon error, otherwise @c EXIT_SUCCESS.
 */
int
main(){

    // Variables for SIOX
    // ------------------

    // Unique interface identifiers
    siox_unique_interface      siox_our_ui;    // This component
    siox_unique_interface      siox_mufs_ui;   // The MUFS layer we will use

    siox_node some_node;

    // This component's identifier
    siox_component *            siox_our_component;
    
    // An identifier for the component-specific activity type 'write'
    siox_component_activity *   siox_act_type_write;

    // Identifiers for the attributes of our activities
    siox_attribute *            siox_att_filename;      // File name
    siox_attribute *            siox_att_bytes2write;   // Bytes to write
    siox_attribute *            siox_att_byteswritten;  // Bytes written

    // An identifier for the actual activity
    siox_activity *             siox_act_write; 

    // An identifier for the remote call we will issue to MUFS
    siox_remote_call *          siox_rc_write;


    // Other variables
    // ---------------

    // Variables for attributes and perfomance data
    unsigned long   bytes_written;  // A collector for performance data
    unsigned long   bytes_to_write; // A collector for a call attribute

    // Variables holding our interface and implementation strings
    char*   swid_s = "SIOX-MockUpFS-Example";
    char*   impl_s = "V2";

    // Variables for accessing MUFS
    char*       mufs_file_name = "EECummings.mufs";
    char        data[] =    "as freedom is a breakfastfood\n"
                            "or truth can live with right and wrong\n"
                            "or molehills are from mountains made\n"
                            "–long enough and just so long\n"
                            "will being pay the rent of seem\n"
                            "and genius please the talentgang\n"
                            "and water most encourage flame\n\n"
                            "as hatracks into peachtrees grow\n"
                            "or hopes dance best on bald men’s hair\n"
                            "and every finger is a toe\n"
                            "and any courage is a fear\n"
                            "–long enough and just so long\n"
                            "will the impure think all things pure\n"
                            "and hornets wail by children stung\n\n"
                            "or as the seeing are the blind\n"
                            "and robins never welcome spring\n"
                            "nor flatfolk prove their world is round\n"
                            "nor dingsters die at break of dong\n"
                            "and common’s rare and millstones float\n"
                            "–long enough and just so long\n"
                            "tomorrow will not be too late\n\n"
                            "worms are the words but joy’s the voice\n"
                            "down shall go which and up come who\n"
                            "breasts will be breasts and thighs will be thighs\n"
                            "deeds cannot dream what dreams can do\n"
                            "–time is a tree (this life one leaf)\n"
                            "but love is the sky and i am for you\n"
                            "just so long and long enough\n";



    /*
     * Info Message
     * ============
     */
    printf( "\n"
            "SIOX Mock-Up File System Example\n"
            "================================\n"
            "\n" );


    /*
     * Checking in with SIOX
     * =====================
     */

    // Look up our interface and implementation info
    siox_our_ui = siox_system_information_lookup_interface_id( swid_s, impl_s );

    // Look up the MUFS layer's interface ("MUFS") and implementation ("OpenMUFS") info
    // This has to be identical to the names the MUFS layer's instrumentation provides!
    siox_mufs_ui = siox_system_information_lookup_interface_id( "MUFS", "OpenMUFS" );

    // Register node itself
    // In place of the NULL, a string may be given as an instance identifier,
    // such as a network port or an MPI rank.
    siox_our_component = siox_component_register( siox_our_ui, "" );

    // Register the activity types we will use
    siox_act_type_write = siox_component_register_activity( siox_our_ui, "write" );

    // Register the attributes we will need
    siox_att_filename = siox_ontology_register_attribute( "filesystem",
                                                          "filename",
                                                          SIOX_STORAGE_STRING );
    siox_att_bytes2write = siox_ontology_register_attribute_with_unit( "io",
                                                                       "data volume/to write",
                                                                       "Bytes",
                                                                       SIOX_STORAGE_64_BIT_INTEGER );
    siox_att_byteswritten = siox_ontology_register_attribute_with_unit( "io",
                                                                        "data volume/written",
                                                                        "Bytes",
                                                                        SIOX_STORAGE_64_BIT_INTEGER );
    /// @todo Mark filename (and b2w?) as descriptors



    /*
     * Writing via MUFS
     * ================
     */

    // Notify SIOX that we are starting an activity of the type we previously declared.
    siox_act_write = siox_activity_start( siox_our_component, siox_act_type_write );

    // Report the imminent (possibly remote) call (and its attributes) of a component with
    // the interface name "MUFS".
    // Here, we use the attributes filename and data volume to characterise the call.

    // First, start a remote call for the activity we just started.
    // Of the parameters indicating the call's target, we use
    //  - the node we will call (our own, found via another siox function),
    //  - the interface ("MUFS"),
    //  - NULL for the associate info, as we have none.
    some_node = siox_lookup_node_id( NULL );
    siox_rc_write = siox_remote_call_setup( siox_act_write,
                                            &some_node,
                                            &siox_mufs_ui,
                                            NULL );
    // Report the call's attributes: The file name we write to.
    siox_remote_call_set_attribute( siox_rc_write, siox_att_filename, mufs_file_name );
    // Report the call's attributes: The amount of data we want to write.
    // Note that here, we use a derived attribute instead of the call's parameters.
    bytes_to_write = strlen( data );
    siox_remote_call_set_attribute( siox_rc_write, siox_att_bytes2write, &bytes_to_write );
    // Inform SIOX that the data for the remote call is complete
    // and the call is about to be submitted.
    siox_remote_call_start( siox_rc_write );

    // The actual call to MUFS to create a file with the given name
    // and write the character data to it.
    mufs_initialize();
    bytes_written = mufs_putfile( mufs_file_name, data );
    if ( bytes_written != (unsigned long) strlen( data ) )
    {
        // Error code to return to SIOX;
        // usually, you'd pass on whatever the lower layer reported.
        siox_activity_error     err_writing = 42;

        fprintf( stderr, "!!! Error writing via MUFS! !!!" );
        // Report to SIOX that the activity ended in an error and disconnect from SIOX.
        siox_activity_report_error( siox_act_write, err_writing );
        siox_activity_end( siox_act_write );
        siox_component_unregister( siox_our_component );
        exit( EXIT_FAILURE );
    }

    // Stop the writing activity;
    // this will stop the clock running on the activity without closing it yet.
    siox_activity_stop( siox_act_write );


    /*
     * Collecting and Reporting Performance Data
     * =========================================
     */

    // Report the activity's performance data we collected.
    // This could take place anytime between siox_start_activity()
    // and siox_end_activity() and for more than one kind of data per activity.
    siox_activity_set_attribute(siox_act_write, siox_att_byteswritten, &bytes_written);


    /*
     * Cleaning up
     * ===========
     */

    // Notify SIOX that all pertinent data has been sent and the activity can be closed
    siox_activity_end( siox_act_write );

    /* Unregister node from SIOX */
    siox_component_unregister( siox_our_component );


    exit( EXIT_SUCCESS );
}

