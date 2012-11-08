/**
 * @file    siox-ll.c
 *          Implementation des SIOX-Low-Level-Interfaces.
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2012
 *          GNU Public License.
 */

/**
 * @mainpage Scalable I/O for Extreme Performance (SIOX)
 *
 * @section intro Introduction
 * 
 * Applications running on HPC systems use a file system to do their I/O. This 
 * mostly consists of the initial read of the input data, the periodical storage 
 * of checkpointing information to restore the execution state from in case of 
 * unexpected program termination, as well as of the eventual writing of the 
 * application's actual output data.
 * 
 * In order for the I/O operations not to become the scalability bottleneck of 
 * HPC applications, the file system and I/O infrastructure must keep pace with 
 * the increasing performance and number of computing cores present on HPC 
 * systems. In this context, a global optimization of the file system turns out 
 * to be very difficult or impossible. In part due to the disparate nature of 
 * the requirements and expectations of different user groups, and in part 
 * because currently there is no way to identify abnormal I/O behavior and trace 
 * it back to its source.
 * 
 * SIOX' main goal is to gain an overview of all the I/O activity taking place 
 * on a HPC system, and to use this information to optimize it. Initially, the 
 * project's scope spans the development of standardized interfaces to collect, 
 * reduce, and store performance data from all relevant layers. This information 
 * will then be analyzed and correlated with previously observed access patterns 
 * in order to gain an understanding of the characteristics and causal 
 * relationships of the system.
 * 
 * This knowledge will be the starting point for subsequent performance 
 * optimizations aimed at specific users and applications, carried out through 
 * e.g. the automatic tuning of Open MPI or file system parameters. Such 
 * use-profiles are going to be continuously created and not only helpful for 
 * optimization, but also when diagnosting acute performance problems, or when 
 * planning new aquisitions. In the course of the project, an holistic approach 
 * for I/O analysis should be conceived, implemented and applied. While SIOX' 
 * applicability is oriented towards HPC environments, it shouldn't be 
 * constricted to them. In this way, the integrated analysis of applications, 
 * file systems, and infrastructure could also be used for the future 
 * optimization of other scenarios e.g. the design of file system caches for 
 * mail servers. 
 * 
 * The following sections describe the current state of the project.
 * 
 * @section architecture SIOX Architecture and Workflow
 * 
 * While the definite architecture of the SIOX system is still subject of 
 * debate, some of its essential components are already well defined. Among them
 * are the SIOX daemons, the knowledge base and the central data warehouse. The
 * workflow between the different components is illustrated in the figure below.
 * 
 * @subsection client SIOX Client
 * 
 * The SIOX client serves as the interface between instrumented software 
 * (libraries and applications) and remote SIOX servers. It is implemented 
 * as a daemon whose job includes the correlation of the local I/O activities, 
 * reduction of redundancy, aggregation of information and the transmission of 
 * relevant performance and trace data to the SIOX servers. 
 * 
 * @subsection servers SIOX Servers
 *
 * SIOX' server functionality is accomplished by a group of separate server 
 * processes fulfilling different roles. 
 *
 * @subsubsection transaction Transaction System
 * 
 * The transaction system is responsible for the continuous collection and 
 * correlation across system boundaries of the flow of trace information sent by 
 * the distributed clients. The result produced by the transaction system is 
 * the causal chain of I/O events produced by an I/O activity at the application
 * layer, as well as a detailed performance evaluation of the relevant links in
 * order to reveal potential I/O bottlenecks. This information is then sent to 
 * the data warehouse server for storage.
 * 
 * @subsubsection datawarehouse Data Warehouse
 * 
 * The data warehouse server manages a persistent archive of historical I/O
 * information and the corresponding observed performance data. It decides 
 * whether the informaiton is worth keeping or not, and if so, it stores it in a
 * database for future consultation. The data warehouse is used periodically by 
 * the knowledge base and the transactions server.
 * 
 * @subsubsection knowledgebase Knowledge Base
 * 
 * The goal of the knowledge base is to capitalize on the historical data 
 * accumulated on the data warehouse to not only hint the user about possible 
 * I/O bottlenecks in his application, but also to autonomously optimize to I/O 
 * subsystem when a well-known I/O pattern has occurred.
 * 
 * @image html architecture.png "SIOX Workflow"
 * 
 * @section intrumentation Sofware Instrumentation
 * 
 * The are two ways to make an application profit from SIOX. The easiest one is 
 * to simply compile the application against a SIOX-enabled library stack. The 
 * other one is to manually instrument the application using the SIOX API 
 * designed for that purpose. Before presenting some examples of manual source
 * code instrumentation there are some fundamental concepts you should 
 * familiarize with.
 * 
 * @subsection nodes SIOX Nodes
 * 
 * A SIOX node is a logical functional unit consisting of hardware components 
 * and a software layer. It represent one of the conceptual links in the 
 * corresponding causal chain of I/O activities. A SIOX node is identified by 
 * the hardware it resides on (e.g. hostname), the software layer it originates
 * from (e.g. application name or library name) and the application's instance
 * (e.g. process or thread id). This identification scheme is very flexible 
 * though and allows the combination of different data to form the different 
 * Id's as long as their concatenation can be used as a unique key. SIOX indexes 
 * all nodes using an unique numeric key. Instrumented applications must 
 * register their SIOX nodes during startup as the first step.
 * 
 * @subsection activities SIOX Activities
 * 
 * A SIOX activity represents an I/O action taking place on a SIOX node. 
 * Activities can be nested and are created by wrapping one or more of the 
 * function calls that trigger I/O. They define the granularity at which the 
 * SIOX system will evalute the I/O events on that particular node.
 * 
 * @subsection activities SIOX Descriptors
 * 
 * 
 * @subsection activities SIOX Metrics
 * 
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <glib.h>

#include "siox-ll.h"
#include "../ontology/ontology.h"

/** Die Default-Ontologie, falls keine andere per siox_set_ontology() gesetzt worden sein sollte. */
#define DEFAULT_ONTOLOGY "testontology"

/** Haben wir initialise_ontology() bereits durchlaufen? */
static bool initialised_ontology = false;

/** Die zu nutzende Ontologie. Kann vor dem ersten Aufruf von initialise_ontology() per siox_set_ontology()
 *  gesetzt werden; ansonsten wird DEFAULT_ONTOLOGY verwendet. */
static char sOntology[80] = "";



/** The next unassigned UNID. */
static unsigned long int    current_unid = 0L;

struct siox_unid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/** The next unassigned AID. */
static unsigned long int    current_aid = 0L;

struct siox_aid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/** The next unassigned DMID. */
static unsigned long int    current_dmid = 0L;

struct siox_dmid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/** The next unassigned RCID. */
static unsigned long int    current_rcid = 0L;

struct siox_rcid_t {
    unsigned long int   id; /**< The actual ID. */
    };


/* typedef gint64  siox_timestamp_t; */


/**
 * Initialisiere die Ontologie-Bibliothek.
 * Wird nur beim ersten Zugriff auf aus der Ontologie zu lesende Daten (etwa die siox_register_...-Funktionen)
 * aufgerufen.
 */
static void initialise_ontology();



siox_unid
siox_register_node( const char * hwid,
                    const char * swid,
                    const char * iid)
{
    /* Draw fresh UNID */
    siox_unid unid = malloc( sizeof( struct siox_unid_t ) );
    (*unid).id = current_unid++;


    printf( "\n# Node registered as UNID %ld with hwid >%s<, swid >%s< and iid >%s<.\n",
        (*unid).id, hwid, swid, iid );

    return( unid );
}


void
siox_unregister_node( siox_unid unid )
{
    printf( "# UNID %ld unregistered.\n\n", (*unid).id );
}


void
siox_node_attribute( siox_unid       unid,
                     siox_dtid      dtid,
                     const void *   value )
{
    printf( "# UNID %ld registered the following additional attributes:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_datatype_get_name( siox_ont_find_datatype_by_dtid( dtid ) ) );
    printf( "%s\n", siox_ont_data_to_string( dtid, value ) );
}


siox_aid
siox_start_activity( siox_unid        unid,
                     siox_timestamp * timestamp )
{
    /** @todo Integrate correct timestamp */
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);

    /* Draw fresh AID */
    siox_aid aid = malloc( sizeof( struct siox_aid_t ) );
    (*aid).id = current_aid++;


    printf( "- UNID %ld started AID %ld at %s",
        (*unid).id, (*aid).id, ctime( &timeStamp ));
    if ( comment != NULL )
        printf( "\tKommentar:\t%s\n", comment );

    return( aid );
}


void
siox_stop_activity( siox_aid    aid,
                    siox_timestamp * time )
{
    /** @todo Integrate correct timestamps */
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);


    printf( "- AID %ld stopped at %s",
        (*aid).id, ctime( &timeStamp ));
}


void
siox_report_activity( siox_aid              aid,
                      siox_mid              mid,
                      void *                value )
{
    printf( "- AID %ld, was measured as follows:\n", aid->id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
}


void
siox_end_activity ( siox_aid          aid,
                    siox_timestamp *  time )
{
    /** @todo Integrate correct timestamps */
    /* Draw timestamp */
    time_t  timeStamp = time(NULL);


    printf( "- AID %ld finally ended at %s\n",
        (*aid).id, ctime( &timeStamp ));
}


void
siox_report( siox_unid              unid,
             siox_mid               mid,
             void *                 value )
{
    printf( "- UNID %ld was measured as follows:\n",
        (*unid).id );
    printf( "\t%s:\t", siox_ont_metric_get_name(
                        siox_ont_find_metric_by_mid( mid ) ) );
    printf( "%s\n", siox_ont_metric_data_to_string( mid, value ) );
}


bool
siox_set_ontology( const char * name )
{
    /* Set ontology name if as yet unset. */
    if( strlen( sOntology ) == 0 )
    {
        strcpy( sOntology, name );
        return( true );
    }
    else
        return( false );
}


siox_dtid
siox_register_datatype( const char *                name,
                        enum siox_ont_storage_type  storage )
{
    siox_dtid       dtid;
    siox_datatype   datatype;

    if( !initialised_ontology )
        initialise_ontology();

    dtid = siox_ont_find_dtid_by_name( name );

    if( dtid )
    {
        /* Compare found metric to current one, returning the mid if they match and NULL otherwise. */
        datatype = siox_ont_find_datatype_by_dtid( dtid );

        if( ( storage != siox_ont_datatype_get_storage( datatype ) ) )
        {
            fprintf( stderr, "ERROR: Could not register the datatype >%s< "
                             "as it exists already,\n"
                             "with different attributes:\n%s\n",
                             name, siox_ont_datatype_to_string( datatype ) );
            return( NULL );
        }
    }
    else
        dtid = siox_ont_register_datatype( name,
                                           storage );

    return( dtid );
}



siox_mid
siox_register_metric( const char *                 name,
                      enum siox_ont_unit_type      unit,
                      enum siox_ont_storage_type   storage,
                      enum siox_ont_scope_type     scope )
{
    siox_mid    mid;
    siox_metric metric;

    if( !initialised_ontology )
        initialise_ontology();

    mid = siox_ont_find_mid_by_name( name );

    if( mid )
    {
        /* Compare found metric to current one, returning the mid if they match and NULL otherwise. */
        metric = siox_ont_find_metric_by_mid( mid );

        if( ( unit != siox_ont_metric_get_unit( metric ) )
            || ( storage != siox_ont_metric_get_storage( metric ) )
            || ( scope != siox_ont_metric_get_scope( metric ) ) )
        {
            fprintf( stderr, "ERROR: Could not register the metric >%s< "
                             "as it exists already,\n"
                             "with different attributes:\n%s\n",
                             name, siox_ont_metric_to_string( metric ) );
            return( NULL );
        }
    }
    else
        mid = siox_ont_register_metric( name,
                                        "(No description yet)",
                                        unit,
                                        storage,
                                        scope );

    return( mid );
}


static void
initialise_ontology()
{
    initialised_ontology = true;

    siox_set_ontology( DEFAULT_ONTOLOGY ); /* Klappt nur, falls noch nicht gesetzt. */

    /* Ontologie öffnen; solange wir keine finalise()-Funktion haben, bleibt sie zu Programmende eben offen. */
    siox_ont_open_ontology( sOntology );
}
