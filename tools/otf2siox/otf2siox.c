/**
 * @page otf2siox OTF2SIOX
 *
 * @section introduction Introduction
 *
 * _otf2siox_ imports a trace in Open Trace Format (OTF) into the
 * SIOX system using the low level interface.
 *
 * @section compiling Compiling
 *
 * There are only two fundamental requirements for compilation:
 *
 * (1) the [OTF library](http://www.tu-dresden.de/die_tu_dresden/zentrale_einrichtungen/zih/forschung/software_werkzeuge_zur_unterstuetzung_von_programmierung_und_optimierung/otf).
 * Version 1.11.1 was used during development.
 *
 * (2) the SIOX low-level API source files.
 *
 * If you want to take advantage of the accompanying make file, you'd also
 * need _cmake_ version 2.8 or newer.
 *
 * @subsection cmake Compiling with cmake
 *
 * (1) create a subdirectory _build_ in the same directory where the source
 * files of _otf2siox_ reside.
 *
 * (2) Inside this subdirectory execute `cmake ..` and then `make`.
 *
 * @section execution Running otf2siox
 *
 * The command line options can be displayd by executing the program with the
 * argument `-h` or `--help`.
 *
 * Please be sure that you generated the OTF file to be imported using a modern
 * version of _VampirTrace_ with the options `VT_IOTRACE` and
 * `VT_IOTRACE_EXTENDED`, otherwise there won't be much information to import.
 *
 * @section note Notes
 *
 * The source files of otf2siox are best viewed with 8 characters tabs.
 *
 * @file otf2siox.c
 * @author Alvaro Aguilera
 */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libotf2siox.h"


/**
 * Shows the usage help.
 *
 * @param progname Name of the _otf2siox_ binary. Same as `argv[0]`.
 */
void usage( const char * progname )
{
	printf( "Usage: %s [OPTION]... [FILE]...\n", progname );
	fputs( "\n\
Imports a trace file into SIOX using the low-level interface. Since OTF does \n\
not provide any information about the computing nodes running the processes, \n\
fictive nodes are created and assigned to the processes according to the     \n\
number of processes per node specified as an argument. The input file must be\n\
in OTF format.\n\
\nOptions:\n\
", stdout );
	fputs( "\
   -h, --help              shows this message.\n\
   -n, --number[=16]       number of processes per node.\n\
   -m, --maxfiles[=1024]   maximal number of OTF event files to open.\n\
   -v, --verbose           verbose output.\n\
", stdout );

}


/**
 * Main function. It handles the command line arguments and starts the import
 * process.
 *
 * @sa import_otf()
 */
int main( int argc, char ** argv )
{
	extern char * optarg;
	extern int opterr;
	extern int optind;

	char c;
	char * filename;
	int hflag = 0, ppn = 2, maxfiles = 1024;
	int option_index = 0;

	static struct option long_options[] = {
		{"help",    no_argument,       0, 'h'},
		{"maxfiles", required_argument, 0, 'm'},
		{"number",  required_argument, 0, 'n'},
		{"verbose", no_argument,       0, 'v'},
		{0,         0,                 0,  0 }
	};

	while( ( c = getopt_long( argc, argv, "hn:m:", long_options,
	                          &option_index ) ) != -1 ) {

		switch( c ) {
			case 'h':
				hflag = 1;
				break;
			case 'm':
				maxfiles = atoi( optarg );
				break;
			case 'n':
				ppn = atoi( optarg );
				break;
			default:
				exit( EINVAL );
		}

	}

	if( hflag ) {
		usage( argv[0] );
		exit( 0 );
	}

	if( optind < argc ) {
		filename = argv[optind];
	} else {
		usage( argv[0] );
		exit( EINVAL );
	}

	import_otf( filename, maxfiles, ppn );

	return 0;
}

