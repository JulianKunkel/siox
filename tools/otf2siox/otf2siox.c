#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libotf2siox.h"


void usage(const char *progname)
{
	printf("Usage: %s [OPTION]... [FILE]...\n", progname);
	fputs("\
Imports a trace file into SIOX using the low-level interface. Since OTF does \n\
not provide any information about the computing nodes running the processes, \n\
fictive nodes are created and assigned to the processes according to the     \n\
number of processes per node specified as an argument. The input file must be\n\
in OTF format.\n\
\nOptions:\n\
", stdout);
	fputs("\
   -h, --help              shows this message.\n\
   -n, --number[=16]       number of processes per node.\n\
   -m, --maxfiles[=1024]   maximal number of OTF event files to open.\n\
   -v, --verbose           verbose output.\n\
", stdout);

}


int main(int argc, char** argv)
{
	extern char *optarg;
	extern int opterr;
	extern int optind;
	
	char c;
	char *filename;
	int hflag = 0, ppn = 2, maxfiles = 1024;
	int option_index = 0;
	
	static struct option long_options[] = {
		{"help",    no_argument,       0, 'h'},
		{"maxfiles",required_argument, 0, 'm'},
		{"number",  required_argument, 0, 'n'},
		{"verbose", no_argument,       0, 'v'},
		{0,         0,                 0,  0 }
	};
	
	while ((c = getopt_long(argc, argv, "hn:m:", long_options, 
		&option_index)) != -1) {
			
		switch (c) {
		case 'h':
			hflag = 1;
			break;
		case 'm':
			maxfiles = atoi(optarg);
			break;
		case 'n':
			ppn = atoi(optarg);
			break;
		default:
			exit(EINVAL);
		}
		
	}
	
	if (hflag) {
		usage(argv[0]);
		exit(0);
	}
	
	if (optind < argc) {
		filename = argv[optind];
	} else {
		perror("Missing input file argument:");
		exit(EINVAL);
	}

	import_otf(filename, maxfiles, ppn);
	
	return 0;
}

