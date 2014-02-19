Requirements
============

The communication code requires cmake 2.8 and the components especified in the CMakeLists.txt 
file, that is:

* Boost library (threads, system, regex, unit_test_framework)
* PostgreSQL devel packages and libpq

During development the following versions were used:

* cmake 2.8
* Boost 1.49
* GCC 4.7 (for C++0x support)
* PostgreSQL 9.2
* Berkeley DB 12.1.6.0.20

Optionally a patched version of Likwid can be used to obtain CPU counters and RAPL energy counters.
See the manual installation.

Additional requirements for the wrappers are found under tools/siox-skeleton-builder/layers


Ubuntu 13.04/13.10
==================
To install the SIOX dependencies call:
apt-get install libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev ninja-build cmake libglib2.0-dev 

To build with postgres you need:
libpqxx3-dev postgresql-common postgresql

In CMAKE set the type folder to: /usr/share/postgresql-common/t


Manual installation of requirements
===================================

To manually install required dependencies download the following software and install it.
The following commands install the dependencies with the default options.

Set variables, this will ensure that the right dependencies are used:
$ export SIOXDEPS=<WHERE YOU WANT TO INSTALL THE DEPS INTO>
$ export PATH=$SIOXDEPS/bin:$PATH
$ export PKG_CONFIG_PATH=$SIOXDEPS/lib/pkgconfig/:$PKG_CONFIG_PATH
$ export LD_LIBRARY_PATH=$SIOXDEPS/lib64:$SIOXDEPS/lib:$LD_LIBRARY_PATH

# build in the following order:
* gcc 4.8.2
	$ bash ./contrib/download_prerequisites
	$ ./configure --prefix=$SIOXDEPS 
	$ make && make install
	# If you get the error: "fatal error: gnu/stubs-32.h: No such file or directory" then you must install the 32 bit libc dev package.
	# if you have exported PATH etc. you will now use the new gcc, check which gcc is used by calling $ which gcc
* cmake 2.8.12 
	$ ./configure --prefix=$SIOXDEPS 
	$ make install
* boost 1.55
	$ ./bootstrap.sh
	$ ./b2 install --prefix=$SIOXDEPS
* libffi (required by Glib2)
 	$ wget ftp://sourceware.org/pub/libffi/libffi-3.0.13.tar.gz	
	$ ./configure --prefix=$SIOXDEPS
	$ make && make install
* Glib2 2.36
	$ ./configure --prefix=$SIOXDEPS
	$ make && make install
* Python (>= 2.7), here we use 3.3.4
	$ ./configure --prefix=$SIOXDEPS
	$ make && make install
	$  ln -s $SIOXDEPS/bin/python3 $SIOXDEPS/bin/python
* MPI (here we use OpenMPI 1.9)
	$ ./configure --prefix=$SIOXDEPS --disable-vt --enable-debug --enable-mem-debug
	$ make && make install
* likwid 		# if you want to use it
	$ hg clone https://code.google.com/p/likwid/ 
	$ cd likwid ; patch -p 1 < ./src/monitoring/statistics/collector/plugins/likwid/likwid-mod.patch
	edit config.mk and adjust prefix
	$ make && make install 
	# Note that make install must be called as root because the permissions of likwid's access deamon  must be setuid
* Berkeley DB	# if you want to use it
	$ wget http://download.oracle.com/berkeley-db/db-6.0.20.tar.gz
	$ tar -xf db-*.tar.gz
	$ cd db*.20/build_unix ; ../dist/configure --prefix=$SIOXDEPS LDFLAGS=-ldl
	$ make && make install

Now to compile SIOX follow the instructions given after calling:
./configure --build-wrappers --with-glib=$SIOXDEPS --with-likwid=$SIOXDEPS --with-boost=$SIOXDEPS --with-cc=$SIOXDEPS --with-cxx=$SIOXDEPS --with-python=$SIOXDEPS


Compilation
===========
By utilizing CMake we can either use the traditional make tool or ninja which has a faster build process (Ubuntu package ninja-build).

Configuration:
We provide a configure script for convenient setting of important CMake variables. So instead of using the options above you may use:
$ ./configure --prefix=/usr/local/siox

The script outputs the command line to execute.

After the installation you must adjust the environment variables LD_LIBRARY_PATH and if you like PATH to allow convenient access, for example for bash do:
$ export LD_LIBRARY_PATH=/usr/local/siox/lib:$LD_LIBRARY_PATH
$ export PATH=/usr/local/siox/bin:$PATH

Then you can compile SIOX:

make -j 4
make install

To use ninja instead of make run:
$ cmake -GNinja ../
$ ninja

Note that C++ takes a considerable amount of memory to compile, parallel builds might exhaust machines with less than 2 GByte of memory.

ccmake is a graphical frontend to cmake which allows easy configuration of the project.



Testing
===========

To execute the unit tests, go into the build dir and run :
make test 
  or
ctest

The output of the tests are then stored in:
<BUILD-DIR>/Testing/Temporary/LastTest.log

To run tests manually, i.e. for debugging purpose, a script is provided which
sets the required LD_LIBRARY_PATH and other environment variables to ease manual execution.

To set the variables correctly run in (the bash)
on level of README.txt:
source ./devel/scripts/set_ld_path.sh




Building, testing & installing the wrappers
===========================================
The wrappers are not built automatically, they are not a part of the global siox build system. With all the consequences.
The wrappers are located under tools/siox-skeleton-builder/layers .
Configuration is the same as for the general siox package, however, you have to supply the --siox option:

	$ cd tools/siox-skeleton-builder/layers/posix
	$ ./waf configure --siox=$INSTALL --prefix=$INSTALL

	$ ./waf build
	$ ./waf --alltests
	$ ./waf install

To build all layers after the installation of SIOX run: ./devel/scripts/build-all-wrappers.sh


To build, test and install waf-based sub-projects:
==================================================
1. Configuration

	$ cd $SUBPROJECT
	$ ./waf configure  --prefix=/usr/local/siox


Patching waf to actually return the output of failing tests to the user:
========================================================================
Note, that you have to have executed ./waf before you can patch it. Don't worry, though, this script is intelligent enough to detect this condition and tell you about it.
Just rerun the patch-waf-output script inside the wrapper directory

	$ $SIOX/scripts/patch-waf-output

2. Building, testing & installing
Note, that while `./waf build` will run some tests, the dependencies of the tests are incomplete due to the modular architecture of siox that waf does not know about.
This will lead both to false positives and false negatives.
To get definit test results, always run the tests with a separate command.

	$ ./waf build
	$ ./waf --alltests
	$ ./waf install




4. Using the wrappers
=====================
4.1 Instrumenting without linking
You can apply any combination of wrappers by setting the LD_PRELOAD environment variable when running the program. This works with any binary, including system commands like `ls`.
To specify more than one preload, separate the paths with a colon ':'.

	$ LD_PRELOAD=$INSTALL/lib/libsiox-posix-dlsym.so ls


4.2 Instrumenting at link-time
To hardcode the siox instrumentation into the executable, simply link against the appropriate wrappers.
Warning: The wrapper should appear right in front of the library it wraps.
If that is not the case, you might end up in a situation where the linker silently throws out either the library or the wrapper, both of which will lead to hard to debug errors.

	$ gcc -o myCoolApp ... -L$INSTALL/lib -lsiox-netcdf-dlsym -lnetcdf ...
	$ ./myCoolApp

4.3 siox-inst
This tool is provided to simplify the task of setting the appropriate flags.
You can invoke it like:
siox-inst [LIST of LAYERS] [wrap|dlsym] [Command]

Specification of dlsym is optional.

For example to achieve LD_PRELOAD linking:
siox-inst posix,mpi dlsym ./testbinary [options]

To achieve compilation at link-time (especially useful for debugging sessions):
siox-inst posix,mpi wrap mpicc -Wall [FILES] -o ./testbinary


5 Reading a trace
When an instrumented application is executed, four files will be created in the current working directory: activities.dat, association.dat, ontology.dat, and system-info.dat
To display the information in a somewhat more human readable format use the siox-trace-reader:

	$ $INSTALL/bin/siox-trace-reader


More about testing:
====================
With waf you can build sample sub modules implementing a particular interface.
Once called the stub will just output function names or append call information to a list (for debugging).
To build go to the directory scripts/dummy-stubs/ and run 
./waf configure build
Internally the script will search for the string "// BUILD_TEST_INTERFACE" inside all include files. 
If found, stubs will be build.

Documentation:
==============

Code documentation is provided inline and can be created invoking
doxygen

For developers there is additional 
