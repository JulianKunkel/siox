aptitude install libpqxx3-dev libpqxx-3.1-dbg



To build, test and install the siox environment, use:

1. Configuration

	$ cd $SIOX
	$ INSTALL=/usr/local/siox
	$ ./waf configure  --prefix=$INSTALL


1.1 Patching waf to actually return the output of failing tests to the user
Note, that you have to have executed ./waf before you can patch it. Don't worry, though, this script is intelligent enough to detect this condition and tell you about it.

	$ ./scripts/patch-waf-output



2. Building, testing & installing
Note, that while `./waf build` will run some tests, the dependencies of the tests are incomplete due to the modular architecture of siox that waf does not know about.
This will lead both to false positives and false negatives.
To get definit test results, always run the tests with a separate command.

	$ ./waf build
	$ ./waf --alltests
	$ ./waf install



3. Building, testing & installing the wrappers
The wrappers are not built automatically, they are not a part of the global siox build system. With all the consequences.
The wrappers are located under tools/siox-skeleton-builder/layers .
Configuration is the same as for the general siox package, however, you have to supply the --siox option:

	$ cd tools/siox-skeleton-builder/layers/posix
	$ ./waf configure --siox=$INSTALL --prefix=$INSTALL

	$ ./waf build
	$ ./waf --alltests
	$ ./waf install


3.1 Patching waf to actually return the output of failing tests to the user:
Just rerun the patch-waf-output script inside the wrapper directory

	$ ../../../../scripts/patch-waf-output



4. Using the wrappers

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



5 Reading a trace
When an instrumented application is executed, four files will be created in the current working directory: activities.dat, association.dat, ontology.dat, and system-info.dat
To display the information in a somewhat more human readable format use the siox-trace-reader:

	$ $INSTALL/bin/siox-trace-reader

