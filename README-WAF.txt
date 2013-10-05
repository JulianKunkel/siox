To build, test and install waf-based sub-projects, use:

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



