Requirements
============

The communication code requires the components especified in the CMakeLists.txt 
file, that is:

* Boost library (threads, system, regex, unit_test_framework)
* PostgreSQL devel packages and libpq

During development the following versions were used:

* Boost 1.49
* GCC 4.7 (for C++0x support)
* PostgreSQL 9.2

Additional requirements for the wrappers are found under tools/siox-skeleton-builder/layers


Ubuntu 13.04/13.10
==================
To install the SIOX dependencies call:
apt-get install libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev ninja-build cmake libglib2.0-dev libpqxx3-dev


Compilation
===========
By utilizing CMake we can either use the traditional make tool or ninja which has a faster build process (Ubuntu package ninja-build).

To compile create a "build" directory and run cmake to create the required files:

$ mkdir build
$ cd build
$ cmake -DBOOST_ROOT=/opt/siox/boost/1.54 -DBOOST_INCLUDEDIR=/opt/siox/boost/1.54/include -DBOOST_LIBRARYDIR=/opt/siox/boost/1.54/lib -DBoost_NO_SYSTEM_PATHS=ON ..

Then you can compile SIOX:
$ make -j 4
$ make install

To use ninja instead of make run:
$ cmake -GNinja ../
$ ninja

Note that C++ takes a considerable amount of memory to compile, parallel builds might exhaust machines with less than 2 GByte of memory.

ccmake is a graphical frontend to cmake which allows easy configuration of the project.

Configure convenient script
===========================

We also provide a configure script for convenient setting of important CMake variables. So instead of using the options above you may use:
$ ./configure --prefix=/usr/local/siox

The script outputs the command line to execute.

After the installation you must adjust the environment variables LD_LIBRARY_PATH and if you like PATH to allow convenient access, for example for bash do:
$ export LD_LIBRARY_PATH=/usr/local/siox/lib:$LD_LIBRARY_PATH
$ export PATH=/usr/local/siox/bin:$PATH


Testing
===========

To execute the unit tests, run :
make test 
  or
ctest

The output of the tests are then stored in:
<BUILD-DIR>/Testing/Temporary/LastTest.log

To run tests manually, i.e. for debugging purpose, a script is provided which
sets the required LD_LIBRARY_PATH and other environment variables to ease manual execution.

To set the variables correctly run in (the bash):
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
